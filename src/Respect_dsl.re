open Respect_domain;

let wrapTest = (fn: Ctx.t => unit) : testFunc =>
  (ctx, callback) =>
    try {
      fn(ctx);
      callback(TestSucceeded)
    } {
    | _ => callback(TestFailed)
    };
type doneCallback = Respect_callbacks.doneCallback;
let wrapW = (fn: (Ctx.t, doneCallback) => unit) : testFunc =>
  (ctx: Ctx.t, callback) =>
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
let it = (name, ex: Ctx.t => unit) => AddExampleOperation(name, wrapTest(ex));
let it_a = (name, ex) => AddExampleOperation(name, ex);
let it_w = (name, ex) => AddExampleOperation(name, wrapW(ex));
let describe = (name, ops) => AddChildGroupOperation(name, ops);
let beforeEach = (fn) => AddSetupOperation(wrapTest(fn));
let beforeEach_w = (fn) => AddSetupOperation(wrapW(fn));
let rec applyOperation = (operation, context, metadata) =>
  switch operation {
  | WrapMetadata((key, value), op) => applyOperation(op, context, metadata |> Ctx.ContextMap.add(key, value))
  | AddSetupOperation(fn) => context |> ExampleGroup.addSetup(Setup(fn))
  | AddExampleOperation(name, func) => {...context, examples: [{name, func, metadata}, ...context.examples]}
  | AddChildGroupOperation(name, ops) =>
    let initial = {...ExampleGroup.empty, name, metadata};
    let newChild = List.fold_left((grp, op) => applyOperation(op, grp, Ctx.ContextMap.empty), initial, ops);
    let newChild' = {...newChild, children: newChild.children |> List.rev, examples: newChild.examples |> List.rev};
    {...context, children: [newChild', ...context.children]}
  };
let applyOperation = (operation, context) => applyOperation(operation, context, Ctx.ContextMap.empty);
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

