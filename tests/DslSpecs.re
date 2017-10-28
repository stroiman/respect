open Respect.Dsl;

open Respect.Matcher;

let dummy = (_) => ();

let parse = (op) =>
  Respect.Domain.ExampleGroup.empty |> applyOperation(op) |> ((grp) => grp.children) |> List.hd;

describe(
  "Dsl",
  [
    it(
      "Example has a setup",
      (_) => {
        let grp = parse(describe("Group", [beforeEach(dummy)]));
        grp.setups |> List.length |> should(equal(1))
      }
    ),
    it(
      "Example group has metadata",
      (_) => {
        let grp = parse(("name", "value") **> describe("Group", [beforeEach(dummy)]));
        let expected =
          TestContext.ContextMap.empty |> TestContext.ContextMap.add("name", Obj.repr("value"));
        grp.metadata |> should(equal(expected))
      }
    ),
    it(
      "Example has metadata",
      (_) => {
        let grp = parse(describe("Group", [("name", "value") **> it("has example", (_) => ())]));
        let expected =
          TestContext.ContextMap.empty |> TestContext.ContextMap.add("name", Obj.repr("value"));
        (grp.examples |> List.hd).metadata |> should(equal(expected))
      }
    ),
    it(
      "Example group has metadata",
      (_) => {
        let grp = parse(("name", "value") **> describe("Group", [beforeEach(dummy)]));
        let expected =
          TestContext.ContextMap.empty |> TestContext.ContextMap.add("name", Obj.repr("value"));
        grp.metadata |> should(equal(expected))
      }
    )
  ]
)
|> register;
