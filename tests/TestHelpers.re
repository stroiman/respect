open Respect.Dsl;

open Respect.Domain;

let passingExampleCode = (_, cb) => cb(TestSucceeded);

let failingExample = (_, cb) => cb(TestFailed);

let anExampleWithCode = (fn) => {
  name: "dummy",
  func: wrapTest(fn),
  metadata: TestContext.ContextMap.empty
};

let anExampleGroup = ExampleGroup.empty;

let withAnExample = ExampleGroup.addExample;

let withExampleCode = (f) => f |> anExampleWithCode |> withAnExample;

let withMetadata = ((name, value), grp) => {
  ...grp,
  metadata: grp.metadata |> TestContext.ContextMap.add(name, value |> Obj.repr)
};

let withSetup = (code) => ExampleGroup.addSetup(Setup(code));

let withChildGroup = (child, grp) => grp |> ExampleGroup.addChild(child);

let withExample = (~metadata=?, ~name="Dummy example", ~code=passingExampleCode, grp) => {
  let md =
    switch metadata {
    | None => TestContext.ContextMap.empty
    | Some((name, value)) =>
      TestContext.ContextMap.empty |> TestContext.ContextMap.add(name, value |> Obj.repr)
    };
  let ex: example = {name, func: code, metadata: md};
  grp |> ExampleGroup.addExample(ex)
};

module MatchHelper = {
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
}
