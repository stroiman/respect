module Matcher = Respect_matcher;

module Callbacks = Respect_callbacks;

/*
   Module Domain describes the internal structure that represents examples
   and groups of examples.
 */
module Domain = {
  /* Represents the outcome of running a test */
  type executionResult =
    | TestSucceeded
    | TestFailed;
  type executionCallback = executionResult => unit;
  /* Internal implementation, a nicer callback is exposed via the DSL */
  type testFunc = TestContext.t => executionCallback => unit;
  type example = {
    name: string,
    func: testFunc
  };
  type setup =
    | Setup testFunc;
  /* A group of examples, and nested groups */
  type exampleGroup = {
    name: string,
    children: list exampleGroup,
    setups: list setup,
    examples: list example
  };
  module ExampleGroup = {
    let empty = {name: "", children: [], setups: [], examples: []};
    let addChild child root => {...root, children: root.children @ [child]};
    let addExample ex grp => {...grp, examples: grp.examples @ [ex]};
    let addSetup code grp => {...grp, setups: grp.setups @ [code]};
  };
};

/*
   The Dsl module contains the constructs you use to describe examples and
   groups, as well as functions to map this to domain types
 */
module Dsl = {
  open Domain;
  let wrapTest (fn: TestContext.t => unit) :testFunc =>
    fun ctx callback =>
      try {
        fn ctx;
        callback TestSucceeded
      } {
      | _ => callback TestFailed
      };
  type doneCallback = Callbacks.doneCallback;
  let wrapW (fn: TestContext.t => doneCallback => unit) :testFunc =>
    fun (ctx: TestContext.t) callback =>
      fn
        ctx
        (
          fun ::err=? () =>
            switch err {
            | None => callback TestSucceeded
            | Some _ => callback TestFailed
            }
        );
  type operation =
    | AddChildGroupOperation string (list operation)
    | AddExampleOperation string testFunc
    | AddSetupOperation testFunc;
  let it name (ex: TestContext.t => unit) => AddExampleOperation name (wrapTest ex);
  let it_a name ex => AddExampleOperation name ex;
  let it_w name ex => AddExampleOperation name (wrapW ex);
  let describe name ops => AddChildGroupOperation name ops;
  let beforeEach fn => AddSetupOperation (wrapTest fn);
  let beforeEach_w fn => AddSetupOperation (wrapW fn);
  let rec applyOperation operation context =>
    switch operation {
      | AddSetupOperation fn => context |> ExampleGroup.addSetup (Setup fn)
      | AddExampleOperation name func => {...context, examples: [{name, func}, ...context.examples]}
      | AddChildGroupOperation name ops =>
      let initial = {...ExampleGroup.empty, name};
      let newChild = List.fold_left (fun grp op => applyOperation op grp) initial ops;
      let newChild' = {
        ...newChild,
          children: newChild.children |> List.rev,
          examples: newChild.examples |> List.rev
          };
      {...context, children: [newChild', ...context.children]}
    };
  let rootContext = ref ExampleGroup.empty;
  let register op => rootContext := !rootContext |> applyOperation op;

  module Async = {
    let it = it_w;
    let describe = describe;
    let register = register;
    let beforeEach = beforeEach_w;
  }
};

module Runner = {
  open Domain;
  open Dsl;
  let mergeResult a b =>
    switch (a, b) {
      | (TestSucceeded, TestSucceeded) => TestSucceeded
      | _ => TestFailed
      };
  let runExample groupStack (ex: example) callback => {
    let ctx = TestContext.create ();
    let doRun () =>
      ex.func ctx (fun r => {
        let str =
          switch r {
            | TestSucceeded => "SUCCESS"
            | TestFailed => "FAILED"
            };
        Js.log ("EXAMPLE: " ^ ex.name ^ " - " ^ str);
        callback r
      });
    let rec runParentGroups grps => {
      switch grps {
        |[] => doRun();
        | [grp, ...parents] => { 
        let rec runSetups = (fun 
            | [] => runParentGroups parents
            | [Setup x, ...rest] => {
            Js.log("Setup for group", grp.name);
            x ctx (fun
                | TestFailed => callback TestFailed
                | TestSucceeded => runSetups rest
                )}
        );
        runSetups grp.setups;
        }
      };
    };
    runParentGroups (groupStack |> List.rev);
    };

  let rec run grp parents callback => {
    let groupStack = [grp, ...parents];
    Js.log ("Entering context " ^ grp.name);
    let rec iter state tests callback => {
      switch tests {
        | [] => callback state
        | [ex, ...rest] =>
        runExample groupStack ex (fun result => iter (mergeResult result state) rest callback)
      };
    };
    let rec iterGrps state grps callback => {
      switch grps {
        | [] => callback state
        | [grp, ...rest] => run grp groupStack (fun result => iterGrps (mergeResult result state) rest callback)
        };
    };
    iter
      TestSucceeded
      grp.examples
      (fun exampleResults => iterGrps exampleResults grp.children (fun x => callback x))
  };
  /* Runs all tests in a single example group. Since a group has no knowledge
     of its parents, using this function will not run setup code registered in
     parents */
  let run grp callback => run grp [] callback;
  /* Runs all tests registered in the root example group */
  let runRoot callback => run !rootContext (fun x => callback x);
};

module TestResult = {
  open Domain;
  let isSuccess result =>
    switch result {
    | TestSucceeded => true
    | TestFailed => false
    };
};
