open Respect.Dsl;

open Respect.Runner;

open Respect.Domain;

open Respect.Matcher;

let exec = (x) => ExampleGroup.empty |> applyOperation(x) |> run;

let beFailure = (result) =>
  switch result {
  | TestFailed => MatchSuccess()
  | _ => MatchFailure(Obj.repr(), Obj.repr())
  };

describe(
  "TestContext",
  [
    describe(
      "Adding data to context",
      [
        it_w(
          "Makes the data retrievable",
          (_) => {
            let ctx = TestContext.create(TestContext.ContextMap.empty);
            ctx 
              |> TestContext.add("key", 42)
              |> TestContext.get("key") 
              |> shoulda(equal(42))
          }
        )
      ]
    )
  ]
)
|> register;

