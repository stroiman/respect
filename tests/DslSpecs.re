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
  }),

  it "Example group has metadata" (fun _ => {
    let grp = parse(
      ("name", "value") **>
      describe "Group" [ beforeEach dummy ]
    );
    let expected = TestContext.ContextMap.empty
      |> TestContext.ContextMap.add "name" (Obj.repr "value");
    grp.metadata |> should (equal expected)
  }),

  it "Example has metadata" (fun _ => {
    let grp = parse(
      describe "Group" [ 
        ("name", "value") **>
        it "has example" (fun _ => ())
      ]
    );
    let expected = TestContext.ContextMap.empty
      |> TestContext.ContextMap.add "name" (Obj.repr "value");
    (grp.examples |> List.hd).metadata |> should (equal expected)
  }),

  it "Example group has metadata" (fun _ => {
    let grp = parse(
      ("name", "value") **>
      describe "Group" [ beforeEach dummy ]
    );
    let expected = TestContext.ContextMap.empty
      |> TestContext.ContextMap.add "name" (Obj.repr "value");
    grp.metadata |> should (equal expected)
  }),
] |> register
