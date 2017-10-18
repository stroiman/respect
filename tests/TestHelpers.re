open Respect.Dsl;

let anExampleWithCode fn => {name: "dummy", func: wrapTest fn};

let anExampleGroup = ExampleGroup.empty;

let withAnExample = ExampleGroup.addExample;

let withExampleCode f => f |> anExampleWithCode |> withAnExample;
