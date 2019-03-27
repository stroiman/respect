open Respect_domain;

/*
   The Dsl module contains the constructs you use to describe examples and
   groups, as well as functions to map this to domain types
 */

let wrapTest = (fn: Respect_ctx.t => unit) : testFunc =>
  (ctx, callback) =>
    try {
      fn(ctx);
      callback(TestSucceeded)
    } {
    | _ => callback(TestFailed)
    };
type doneCallback = Respect_callbacks.doneCallback;
let wrapW = (fn: (Respect_ctx.t, doneCallback) => unit) : testFunc =>
  (ctx: Respect_ctx.t, callback) =>
    fn(
      ctx,
      (~err=?, ()) =>
        switch err {
        | None => callback(TestSucceeded)
        | Some(_) => callback(TestFailed)
        }
    );

type operator = 
| Focus;
type operation =
  | AddOperator(operator, operation)
  | WrapMetadata((string, Obj.t), operation)
  | AddChildGroupOperation(string, list(operation))
  | AddExampleOperation(string, testFunc)
  | AddSetupOperation(testFunc);
let it = (name, ex: Respect_ctx.t => unit) => AddExampleOperation(name, wrapTest(ex));
let it_a = (name, ex) => AddExampleOperation(name, ex);
let it_w = (name, ex) => AddExampleOperation(name, wrapW(ex));
let describe = (name, ops) => AddChildGroupOperation(name, ops);
let beforeEach = (fn) => AddSetupOperation(wrapTest(fn));
let beforeEach_w = (fn) => AddSetupOperation(wrapW(fn));
let rec applyOperation = (operation, context, metadata, focus) =>
  switch operation {
  | AddOperator(Focus, op) => applyOperation(op, context, metadata, true)
  | WrapMetadata((key, value), op) => applyOperation(op, context, metadata |> Respect_ctx.ContextMap.add(key, value), true)
  | AddSetupOperation(fn) => context |> ExampleGroup.addSetup(Setup(fn))
  | AddExampleOperation(name, func) => {...context, examples: [{name, func, metadata, focused: focus}, ...context.examples]}
  | AddChildGroupOperation(name, ops) =>
    let initial = {...ExampleGroup.empty, name, metadata};
    let newChild = List.fold_left((grp, op) => applyOperation(op, grp, Respect_ctx.ContextMap.empty, focus), initial, ops);
    let newChild' = {...newChild, children: newChild.children |> List.rev, examples: newChild.examples |> List.rev};
    {...context, children: [newChild', ...context.children]}
  };
let applyOperation = (operation, context) => applyOperation(operation, context, Respect_ctx.ContextMap.empty, false);
let rootContext = ref(ExampleGroup.empty);
let register = (op) => rootContext := rootContext^ |> applyOperation(op);
let ( **> ) = ((key, value), op) => WrapMetadata((key, Obj.repr(value)), op);
let focus = op => AddOperator(Focus, op); 

module Example = {
  let isFocused = (x: example) => x.focused;
};

module Async = {
  let ( **> ) = ( **> );
  let focus = focus;
  let it = it_w;
  let describe = describe;
  let register = register;
  let beforeEach = beforeEach_w;
  let pending = name => AddExampleOperation(name, (_,cb) => cb(TestPending));
};

