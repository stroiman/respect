open Respect.Dsl;
open Respect.Matcher;

let dummy = fun _ => ();

let parse op => 
  Respect.Domain.ExampleGroup.empty 
  |> applyOperation op 
  |> (fun grp => grp.children)
  |> List.hd;

describe "Dsl" [
  it "Example has a setup" (fun _ => {
    let grp = parse(
      describe "Group" [ beforeEach dummy ]
    );
    grp.setups |> List.length |> should (equal 1)
  })
] |> register
