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
  let runExample (ex: example) =>
    try {
      TestContext.create () |> ex.func;
      Js.log (ex.name ^ " - SUCCESS");
      TestSucceeded
    } {
    | _ =>
      Js.log (ex.name ^ " - FAILED");
      TestFailed
    };
  let rec run ctx :executionResult => {
    let dorun ctx => {
      let result =
        ctx.examples |>
        List.fold_left
          (
            fun a ex => {
              let b = ex |> runExample;
              mergeResult a b
            }
          )
          TestSucceeded;
      ctx.children |>
      List.fold_left
        (
          fun a x => {
            let b = run x;
            mergeResult a b
          }
        )
        result
    };
    let result =
      try (dorun ctx) {
      | _ => TestFailed
      };
    result
  };
  let runRoot callback => callback (!rootContext |> run);
};

module TestResult = {
  open Runner;
  let isSuccess result =>
    switch result {
    | TestSucceeded => true
    | TestFailed => false
    };
};
