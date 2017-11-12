module Matcher = Respect_matcher;
module MatchersV2 = Respect_matchersV2;

module Callbacks = Respect_callbacks;

module As = Respect_async;

/*
   Module Domain describes the internal structure that represents examples
   and groups of examples.
 */
module Domain = {
  /* Represents the outcome of running a test */
  type executionResult =
    | TestPending
    | TestSucceeded
    | TestFailed;
  type executionCallback = executionResult => unit;
  /* Internal implementation, a nicer callback is exposed via the DSL */
  type testFunc = (TestContext.t, executionCallback) => unit;
  type example = {
    name: string,
    func: testFunc,
    metadata: TestContext.contextMap
  };
  type setup =
    | Setup(testFunc);
  /* A group of examples, and nested groups */
  type exampleGroup = {
    name: string,
    children: list(exampleGroup),
    setups: list(setup),
    examples: list(example),
    metadata: TestContext.contextMap
  };
  module ExampleGroup = {
    let empty = {name: "", children: [], setups: [], examples: [], metadata: TestContext.ContextMap.empty};
    let addChild = (child, root) => {...root, children: root.children @ [child]};
    let addExample = (ex, grp) => {...grp, examples: grp.examples @ [ex]};
    let addSetup = (code, grp) => {...grp, setups: grp.setups @ [code]};
  };
};

/*
   The Dsl module contains the constructs you use to describe examples and
   groups, as well as functions to map this to domain types
 */
module Dsl = {
  open Domain;
  let wrapTest = (fn: TestContext.t => unit) : testFunc =>
    (ctx, callback) =>
      try {
        fn(ctx);
        callback(TestSucceeded)
      } {
      | _ => callback(TestFailed)
      };
  type doneCallback = Callbacks.doneCallback;
  let wrapW = (fn: (TestContext.t, doneCallback) => unit) : testFunc =>
    (ctx: TestContext.t, callback) =>
      fn(
        ctx,
        (~err=?, ()) =>
          switch err {
          | None => callback(TestSucceeded)
          | Some(_) => callback(TestFailed)
          }
      );
  type operation =
    | WrapMetadata((string, Obj.t), operation)
    | AddChildGroupOperation(string, list(operation))
    | AddExampleOperation(string, testFunc)
    | AddSetupOperation(testFunc);
  let it = (name, ex: TestContext.t => unit) => AddExampleOperation(name, wrapTest(ex));
  let it_a = (name, ex) => AddExampleOperation(name, ex);
  let it_w = (name, ex) => AddExampleOperation(name, wrapW(ex));
  let describe = (name, ops) => AddChildGroupOperation(name, ops);
  let beforeEach = (fn) => AddSetupOperation(wrapTest(fn));
  let beforeEach_w = (fn) => AddSetupOperation(wrapW(fn));
  let rec applyOperation = (operation, context, metadata) =>
    switch operation {
    | WrapMetadata((key, value), op) => applyOperation(op, context, metadata |> TestContext.ContextMap.add(key, value))
    | AddSetupOperation(fn) => context |> ExampleGroup.addSetup(Setup(fn))
    | AddExampleOperation(name, func) => {...context, examples: [{name, func, metadata}, ...context.examples]}
    | AddChildGroupOperation(name, ops) =>
      let initial = {...ExampleGroup.empty, name, metadata};
      let newChild = List.fold_left((grp, op) => applyOperation(op, grp, TestContext.ContextMap.empty), initial, ops);
      let newChild' = {...newChild, children: newChild.children |> List.rev, examples: newChild.examples |> List.rev};
      {...context, children: [newChild', ...context.children]}
    };
  let applyOperation = (operation, context) => applyOperation(operation, context, TestContext.ContextMap.empty);
  let rootContext = ref(ExampleGroup.empty);
  let register = (op) => rootContext := rootContext^ |> applyOperation(op);
  let ( **> ) = ((key, value), op) => WrapMetadata((key, Obj.repr(value)), op);
  module Async = {
    let ( **> ) = ( **> );
    let it = it_w;
    let describe = describe;
    let register = register;
    let beforeEach = beforeEach_w;
    let pending = name => AddExampleOperation(name, (_,cb) => cb(TestPending));
  };
};

module Runner = {
  open Domain;
  open Dsl;
  let mergeResult = (a, b) =>
    switch (a, b) {
    | (TestFailed, _) => TestFailed
    | (_, TestFailed) => TestFailed
    | (TestPending, _) => TestPending
    | (_, TestPending) => TestPending
    | (TestSucceeded, TestSucceeded) => TestSucceeded
    };
  let runExample = (groupStack, ex: example) : As.t(executionResult) => {
    let ctx = {
      let mdStack = groupStack |> List.map((x) => x.metadata);
      let mdStack' = [ex.metadata, ...mdStack];
      let md =
        List.fold_left(TestContext.ContextMap.merge, TestContext.ContextMap.empty, mdStack');
      TestContext.create(md);
    };

    let logError = r => {
      if (r == TestPending) {
        let groupNames = List.fold_left (((acc, grp) =>
          if (grp.name == "") { acc } else {
        grp.name ++ " - " ++ acc}), "", groupStack);
          Js.log("EXAMPLE: " ++ groupNames ++ (ex.name ++ " - PENDING"));
        };
      if (r == TestFailed) {
        let groupNames = List.fold_left (((acc, grp) =>
          if (grp.name == "") { acc } else {
        grp.name ++ " - " ++ acc}), "", groupStack);
          Js.log("EXAMPLE: " ++ groupNames ++ (ex.name ++ " - FAILED"));
        };
      r;
    };

    let doRun = () => ex.func(ctx) |> As.from_callback;

    let rec runParentGroups = (grps) : As.t(executionResult) =>
      switch grps {
      | [] => doRun()
      | [grp, ...parents] =>
        let rec runSetups = (setups) : As.t(executionResult) => 
          switch(setups) {
          | [] => runParentGroups(parents)
          | [Setup(x), ...rest] => 
              x(ctx) |> As.from_callback |> As.bind(
                ~f=fun
                | TestFailed => As.return(TestFailed)
                | TestPending => As.return(TestPending)
                | TestSucceeded => runSetups(rest)
              )
          };
        runSetups(grp.setups)
      };
    runParentGroups(groupStack |> List.rev)
      |> As.timeout(As.Seconds(1))
      |> As.tryCatch(~f=(_) => Some(TestFailed))
      |> As.map(~f=logError);
  };
  let rec run = (grp, parents) : As.t(executionResult) => {
    let groupStack = [grp, ...parents];
    let rec iter = (state, tests) : As.t(executionResult) =>
      switch tests {
      | [] => As.return(state)
      | [ex, ...rest] => runExample(groupStack, ex) |> As.bind(~f=result => iter(mergeResult(result, state), rest))
      };
    let rec iterGrps = (state, grps) : As.t(executionResult) =>
      switch grps {
      | [] => As.return(state)
      | [grp, ...rest] => run(grp, groupStack) |> As.bind(~f=(result) => iterGrps(mergeResult(result, state), rest))
      };
    iter(TestSucceeded, grp.examples) |> As.bind(~f=(exampleResults) => iterGrps(exampleResults, grp.children))
  };
  /* Runs all tests in a single example group. Since a group has no knowledge
     of its parents, using this function will not run setup code registered in
     parents */
  let run = (grp) : As.t(executionResult) => run(grp, []);
  /* Runs all tests registered in the root example group */
  let runRoot = () : As.t(executionResult) => run(rootContext^);
};

module TestResult = {
  open Domain;
  let isSuccess = (result) =>
    switch result {
    | TestSucceeded => true
    | TestPending => true
    | TestFailed => false
    };
};
