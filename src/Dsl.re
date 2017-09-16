type testFunc = unit => unit;

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

let rec run ctx => {
  ctx.examples |> List.iter (fun ex => ex.func ());
  ctx.children |> List.iter (fun x => run x)
};
