open Respect;
open Respect.Domain;

let passingExample = (~onRun=?,()) => (ctx, cb) => {
  switch(onRun) {
    | Some(f) => f(ctx);
    | None => ();
  };
  cb(TestSucceeded);
};

let failingExample = (~onRun=?,()) => (ctx, cb) => {
  switch(onRun) {
    | Some(f) => f(ctx);
    | None => ();
  };
  cb(TestFailed);
};

let pendingExample = (~onRun=?,()) => (ctx, cb) => {
  switch(onRun) {
    | Some(f) => f(ctx);
    | None => ();
  };
  cb(TestPending);
};

let anExampleWithCode = (fn) => {
  name: "dummy",
  func: Dsl.wrapTest(fn),
  metadata: Ctx.ContextMap.empty
};

let passingSetup = passingExample;

let anExampleGroup = ExampleGroup.empty;

let withAnExample = ExampleGroup.addExample;

let withExampleCode = (f) => f |> anExampleWithCode |> withAnExample;

let withMetadata = ((name, value), grp) => {
  ...grp,
  metadata: grp.metadata |> Ctx.ContextMap.add(name, value |> Obj.repr)
};

let withSetup = (code) => ExampleGroup.addSetup(Setup(code));

let withChildGroup = (child, grp) => grp |> ExampleGroup.addChild(child);

let withExample = (~metadata=?, ~name="Dummy example", ~code=passingExample(), grp) => {
  let md =
    switch metadata {
    | None => Ctx.ContextMap.empty
    | Some((name, value)) =>
      Ctx.ContextMap.empty |> Ctx.ContextMap.add(name, value |> Obj.repr)
    };
  let ex: example = {name, func: code, metadata: md};
  grp |> ExampleGroup.addExample(ex)
};

module MatchHelper = {
  open Respect_callbacks;
  let shouldMatch = (fn:doneCallback => unit) => (don:doneCallback) => {
  fn(fun (~err=?,()) => {
    switch(err) {
      | None => don();
      | Some(x) => don(~err=x,());
    }
  })
};

let shouldNotMatch = (fn:doneCallback => unit) => (don:doneCallback) => {
  fn(fun (~err=?,()) => {
    switch(err) {
      | None => don(~err="Expected match error, but none was received",());
      | Some(_) => don();
    }
  })
};
};

module AsyncMatchers = {
  open Respect.Matcher;

  let asyncResolve = (actual:Async.t('a)) => cb => {
    let successCb = x => cb(MatchSuccess(x));
    let exnCb = x => cb(MatchFailure(x |> Obj.repr,x |> Obj.repr));
    actual |> Async.run(successCb,~fe=exnCb)
  };

  let asyncThrow = (actual:Async.t('a)) => cb => {
    let successCb = x => cb(MatchFailure(x |> Obj.repr,x |> Obj.repr));
    let exnCb = x => cb(MatchSuccess(x));
    actual |> Async.run(successCb,~fe=exnCb)
  };
}
