open Respect.Dsl;

open Respect.Domain;

let passingExampleCode = fun _ cb => cb TestSucceeded;
let failingExample = fun _ cb => cb TestFailed;

let anExampleWithCode fn => {name: "dummy", func: wrapTest fn};

let anExampleGroup = ExampleGroup.empty;

let withAnExample = ExampleGroup.addExample;

let withExampleCode f => f |> anExampleWithCode |> withAnExample;

let withSetup code => {
  ExampleGroup.addSetup (Setup code);
};

let withExample ::name="Dummy example" ::code=passingExampleCode grp => {
  let ex : example =  { name: name, func: code };
  grp |> ExampleGroup.addExample ex;
}
