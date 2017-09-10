type testFunc = unit => unit;

type test = {
  name: string,
  func: testFunc
};

type setup =
  | Setup testFunc;

type testContext = {
  name: string,
  children: list testContext,
  setups: list setup,
  tests: list test
};

type operation =
  | AddContextOperation string (list operation)
  | AddTestOperation string testFunc;

let it name test => AddTestOperation name test;

let describe name ops => AddContextOperation name ops;

module TestContext = {
  let empty = {name: "", children: [], setups: [], tests: []};
  let addChild child root => {...root, children: root.children @ [child]};
};

let rec applyOperation operation context =>
  switch operation {
  | AddTestOperation name func => {...context, tests: [{name, func}, ...context.tests]}
  | AddContextOperation name ops =>
    let initial = {...TestContext.empty, name};
    let newChild = List.fold_left (fun ctx op => applyOperation op ctx) initial ops;
    let newChild' = {
      ...newChild,
      children: newChild.children |> List.rev,
      tests: newChild.tests |> List.rev
    };
    {...context, children: [newChild', ...context.children]}
  };

let rootContext = ref TestContext.empty;

let register op => rootContext := !rootContext |> applyOperation op;

let rec run ctx => {
  ctx.tests |> List.iter (fun test => test.func ());
  ctx.children |> List.iter (fun x => run x)
};
