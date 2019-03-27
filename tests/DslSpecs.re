open Respect;
open Respect.Dsl;
open Respect.Matcher;

let dummy = _ => ();

let parse = op =>
  Respect.Domain.ExampleGroup.empty
  |> applyOperation(op)
  |> (grp => grp.children)
  |> List.hd;

let exampleCode = _ => ();

describe(
  "Dsl",
  [
    it("Example has a setup", _ => {
      let grp = parse(describe("Group", [beforeEach(dummy)]));
      grp.setups |> List.length |> should(equal(1));
    }),
    it("Example group has metadata", _ => {
      let grp =
        parse(
          ("name", "value") **> describe("Group", [beforeEach(dummy)]),
        );
      let expected =
        Ctx.ContextMap.empty |> Ctx.ContextMap.add("name", Obj.repr("value"));
      grp.metadata |> should(equal(expected));
    }),
    it("Example has metadata", _ => {
      let grp =
        parse(
          describe(
            "Group",
            [("name", "value") **> it("has example", exampleCode)],
          ),
        );
      let expected =
        Ctx.ContextMap.empty |> Ctx.ContextMap.add("name", Obj.repr("value"));
      (grp.examples |> List.hd).metadata |> should(equal(expected));
    }),
    it("Example group has metadata", _ => {
      let grp =
        parse(
          ("name", "value") **> describe("Group", [beforeEach(dummy)]),
        );
      let expected =
        Ctx.ContextMap.empty |> Ctx.ContextMap.add("name", Obj.repr("value"));
      grp.metadata |> should(equal(expected));
    }),
    describe(
      "Skipped",
      [
        it("is true when example has 'skip @@'", _ => {
          let grp =
            parse(
              describe("group", [skip @@ it("has example", exampleCode)]),
            );
          grp.examples
          |> List.map(Domain.Example.isSkipped)
          |> should(equal([true]));
        }),
        it("is false when example doesn't have `skip @@`", _ => {
          let grp =
            parse(describe("group", [it("has example", exampleCode)]));
          grp.examples
          |> List.map(Domain.Example.isSkipped)
          |> should(equal([false]));
        }),
      ],
    ),describe(
      "Focused",
      [
        it("is true when example has 'focus @@'", _ => {
          let grp =
            parse(
              describe("group", [focus @@ it("has example", exampleCode)]),
            );
          grp.examples
          |> List.map(Example.isFocused)
          |> should(equal([true]));
        }),
        it("is false when example doesn't have `focus @@`", _ => {
          let grp =
            parse(describe("group", [it("has example", exampleCode)]));
          grp.examples
          |> List.map(Example.isFocused)
          |> should(equal([false]));
        }),
      ],
    ),
  ],
)
|> register;
