module Matcher = Respect_matcher;

module Dsl = {
  type testFunc = TestContext.t => unit;
  type example = {
    name: string,
    func: testFunc
  };
  type setup =
    | Setup testFunc;
  type exampleGroup = {
    name: string,
    children: list exampleGroup,
    setups: list setup,
    examples: list example
  };
  type operation =
    | AddContextOperation string (list operation)
    | AddExampleOperation string testFunc;
  let it name ex => AddExampleOperation name ex;
  let describe name ops => AddContextOperation name ops;
  module ExampleGroup = {
    let empty = {name: "", children: [], setups: [], examples: []};
    let addChild child root => {...root, children: root.children @ [child]};
    let addExample ex grp => {...grp, examples: grp.examples @ [ex]};
  };
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
  open Dsl;
  type executionResult =
    | TestSucceeded
    | TestFailed;
  let mergeResult a b =>
    switch (a, b) {
    | (TestSucceeded, TestSucceeded) => TestSucceeded
    | _ => TestFailed
    };
  let runExample (ex: example) callback =>
    try {
      TestContext.create () |> ex.func;
      Js.log (ex.name ^ " - SUCCESS");
      callback TestSucceeded
    } {
    | _ =>
      Js.log (ex.name ^ " - FAILED");
      callback TestFailed
    };
  let run ctx callback => {
    let rec doRun ctx => {
      let r = ref TestSucceeded;
      let rec iter state tests callback =>
        switch tests {
        | [] => callback state
        | [ex, ...rest] =>
          runExample ex (fun result => iter (mergeResult result state) rest callback)
        };
      iter TestSucceeded ctx.examples (fun x => r := x);
      let result = !r;
      ctx.children |>
      List.fold_left
        (
          fun a x => {
            let b = doRun x;
            mergeResult a b
          }
        )
        result
    };
    callback (doRun ctx)
  };
  let runRoot callback => run !rootContext (fun x => callback x);
};

module TestResult = {
  open Runner;
  let isSuccess result =>
    switch result {
    | TestSucceeded => true
    | TestFailed => false
    };
};
