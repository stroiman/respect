open Respect.Dsl;

open Respect.Runner;

open Respect.Domain;

open Respect.Matcher;

let exec x => ExampleGroup.empty |> applyOperation x |> run;

let beFailure result =>
  switch result {
  | Respect.Domain.TestSucceeded => MatchFailure (Obj.repr ())
  | Respect.Domain.TestFailed => MatchSuccess ()
  };

describe
  "TestContext"
  [
    describe
      "Adding data to context"
      [
        it
          "Makes the data retrievable"
          (
            fun _ => {
              let ctx = TestContext.create ();
              ctx |> TestContext.add "key" 42;
              ctx |> TestContext.get "key" |> should (equal 42)
            }
          )
      ]
  ] |> register;

describe
  "Async tests"
  [
    it_a
      "has an async test"
      (
        fun _ cb => {
          let work () => cb TestSucceeded;
          Js.Global.setTimeout work 10 |> ignore
        }
      )
  ] |> register;
