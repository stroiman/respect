module Matcher = Respect_matcher;

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
  type doneCallback = err::string? => unit => unit;
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
    | AddContextOperation string (list operation)
    | AddExampleOperation string testFunc;
  let it name (ex: TestContext.t => unit) => AddExampleOperation name (wrapTest ex);
  let it_a name ex => AddExampleOperation name ex;
  let it_w name ex => AddExampleOperation name (wrapW ex);
  let describe name ops => AddContextOperation name ops;
  let rec applyOperation operation context =>
    switch operation {
    | AddExampleOperation name func => {...context, examples: [{name, func}, ...context.examples]}
    | AddContextOperation name ops =>
      let initial = {...ExampleGroup.empty, name};
      let newChild = List.fold_left (fun ctx op => applyOperation op ctx) initial ops;
      let newChild' = {
        ...newChild,
        children: newChild.children |> List.rev,
        examples: newChild.examples |> List.rev
      };
      {...context, children: [newChild', ...context.children]}
    };
  let rootContext = ref ExampleGroup.empty;
  let register op => rootContext := !rootContext |> applyOperation op;
};

module Runner = {
  open Domain;
  open Dsl;
  let mergeResult a b =>
    switch (a, b) {
      | (TestSucceeded, TestSucceeded) => TestSucceeded
      | _ => TestFailed
      };
  let runExample (grp:exampleGroup) (ex: example) callback => {
    let ctx = TestContext.create ();
    let doRun () =>{
      ex.func
        ctx
        (
          fun r => {
            let str =
              switch r {
                | TestSucceeded => "SUCCESS"
                | TestFailed => "FAILED"
                };
            Js.log ("EXAMPLE: " ^ ex.name ^ " - " ^ str);
            callback r
          }
        );
    };
    switch(grp.setups) {
      | [] => doRun();
      | [(Setup x), ..._] => {
        x ctx (fun result => {
          switch(result) {
          | TestFailed => callback TestFailed;
          | TestSucceeded => doRun();
          }
        });
      };
    }
  };
  let rec run ctx callback => {
    Js.log ("Entering context " ^ ctx.name);
    let rec iter state tests callback =>
      switch tests {
        | [] => callback state
        | [ex, ...rest] =>
        runExample ctx ex (fun result => iter (mergeResult result state) rest callback)
      };
    let rec iterGrps state grps callback =>
      switch grps {
        | [] => callback state
        | [grp, ...rest] => run grp (fun result => iterGrps (mergeResult result state) rest callback)
        };
    iter
      TestSucceeded
      ctx.examples
      (fun exampleResults => iterGrps exampleResults ctx.children (fun x => callback x))
  };
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
