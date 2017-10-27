open Respect.Dsl;

open Respect.Domain;

let passingExampleCode = fun _ cb => cb TestSucceeded;
let failingExample = fun _ cb => cb TestFailed;

let anExampleWithCode fn => {name: "dummy", func: wrapTest fn, metadata:
  TestContext.ContextMap.empty};

let anExampleGroup = ExampleGroup.empty;

let withAnExample = ExampleGroup.addExample;

let withExampleCode f => f |> anExampleWithCode |> withAnExample;

let withMetadata (name,value) grp => {...grp, metadata: grp.metadata |>
TestContext.ContextMap.add name (value |> Obj.repr)};

let withSetup code => {
  ExampleGroup.addSetup (Setup code);
};

let withChildGroup child grp => grp |> ExampleGroup.addChild child;

let withExample ::metadata=? ::name="Dummy example" ::code=passingExampleCode grp => {
  let md = switch metadata {
    | None => TestContext.ContextMap.empty;
    | Some (name,value) => TestContext.ContextMap.empty
      |> TestContext.ContextMap.add name (value |> Obj.repr);
  };
  let ex : example =  { name: name, func: code, metadata: md };
  grp |> ExampleGroup.addExample ex;
}
