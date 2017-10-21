open Respect.Dsl;

open Respect.Runner;

open Respect.Domain;

open Respect.Matcher;

open TestHelpers;

let exec x => ExampleGroup.empty |> applyOperation x |> run;

exception TestFailed string;

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

describe
  "Runner"
  [
    describe
      "example throws an exception"
      [
        it
          "returns an error message"
          (
            fun _ => {
              let result = ref TestSucceeded;
              let ex = anExampleGroup |> withExampleCode (fun _ => raise (TestFailed ""));
              run ex (fun x => result := x);
              !result |> should beFailure
            }
          )
      ] /*it "returns an error message" (fun _ => {*/,
    /*anExampleGroup |> withExampleCode (fun _ => append "x") |> run;*/
    /*});*/
    it
      "executes the example code"
      (
        fun _ => {
          let lines = ref [];
          let append line => lines := [line, ...!lines];
          let ex = anExampleGroup |> withExampleCode (fun _ => append "x");
          run ex ignore;
          !lines |> should (equal ["x"])
        }
      )
  ] |> register;
