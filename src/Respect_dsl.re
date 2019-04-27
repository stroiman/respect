open Respect_domain;

let wrapTest = (fn: Respect_ctx.t => unit): testFunc =>
  (ctx, callback) =>
    try (
      {
        fn(ctx);
        callback(TestSucceeded);
      }
    ) {
    | _ => callback(TestFailed)
    };
type doneCallback = Respect_callbacks.doneCallback;
let wrapW = (fn: (Respect_ctx.t, doneCallback) => unit): testFunc =>
  (ctx: Respect_ctx.t, callback) =>
    fn(ctx, (~err=?, ()) =>
      switch (err) {
      | None => callback(TestSucceeded)
      | Some(_) => callback(TestFailed)
      }
    );

type operator =
  | Skip
  | Focus;

type operation =
  | AddOperator(operator, operation)
  | WrapMetadata((string, Obj.t), operation)
  | AddChildGroupOperation(string, list(operation))
  | AddExampleOperation(string, testFunc)
  | AddSetupOperation(testFunc);

let rec applyOperation = (operation, context, metadata, focus, skipped) =>
  switch (operation) {
  | AddOperator(Focus, op) =>
    applyOperation(op, context, metadata, true, skipped)
  | AddOperator(Skip, op) =>
    applyOperation(op, context, metadata, focus, true)
  | WrapMetadata((key, value), op) =>
    applyOperation(
      op,
      context,
      metadata |> Respect_ctx.ContextMap.add(key, value),
      focus,
      skipped,
    )
  | AddSetupOperation(fn) => context |> ExampleGroup.addSetup(Setup(fn))
  | AddExampleOperation(name, func) => {
      ...context,
      examples: [
        {name, func, metadata, focused: focus, skipped},
        ...context.examples,
      ],
    }
  | AddChildGroupOperation(name, ops) =>
    let initial = {...ExampleGroup.empty, name, metadata};
    let newChild =
      List.fold_left(
        (grp, op) =>
          applyOperation(
            op,
            grp,
            Respect_ctx.ContextMap.empty,
            focus,
            skipped,
          ),
        initial,
        ops,
      );
    let newChild' = {
      ...newChild,
      children: newChild.children |> List.rev,
      examples: newChild.examples |> List.rev,
    };
    {...context, children: [newChild', ...context.children]};
  };
let applyOperation = (operation, context) =>
  applyOperation(
    operation,
    context,
    Respect_ctx.ContextMap.empty,
    false,
    false,
  );
let rootContext = ref(ExampleGroup.empty);

module Example = {
  let isFocused = (x: example) => x.focused;
};

module type DslMapper = {
  type t('a);
  let mapTestfunc: (Respect_ctx.t => t('a)) => testFunc;
};

module Make = (M: DslMapper) => {
  type t('a) = M.t('a);
  let it = (name, f) => AddExampleOperation(name, M.mapTestfunc(f));
  let describe = (name, ops) => AddChildGroupOperation(name, ops);
  let beforeEach = f => AddSetupOperation(M.mapTestfunc(f));
  let ( **> ) = ((key, value), op) =>
    WrapMetadata((key, Obj.repr(value)), op);
  let focus = op => AddOperator(Focus, op);
  let skip = op => AddOperator(Skip, op);
  let pending = name =>
    AddExampleOperation(name, (_, cb) => cb(TestPending));
  let register = op => rootContext := rootContext^ |> applyOperation(op);
};

module SyncMapper = {
  type t('a) = unit;
  let mapTestfunc = wrapTest;
};

module AsyncMapper = {
  type t('a) = doneCallback => unit;
  let mapTestfunc = (fn: (Respect_ctx.t) => t('a)): testFunc =>
    (ctx: Respect_ctx.t, callback) =>
      fn(ctx, (~err=?, ()) =>
        switch (err) {
        | None => callback(TestSucceeded)
        | Some(_) => callback(TestFailed)
        }
      );
};

module Sync = Make(SyncMapper);
include Sync;

module Async = {
  include Make(AsyncMapper);

  let register = register;
};
