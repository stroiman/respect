open Respect;
open Respect.Dsl.Async;
open Respect.Matcher;

describe(
  "Async dsl",
  [
    it( "Example group has metadata",
      (_) => {
        let grp = DslSpecs.parse(("name", "value") **> describe("Group", []));
        let expected = Ctx.ContextMap.empty |> Ctx.ContextMap.add("name", Obj.repr("value"));
        grp.metadata |> shoulda(equal(expected))
      }
    )
  ]
)
|> register;
