open Respect.Dsl;

open Respect.Domain;

open Respect.Runner;

open Respect.Matcher;

open TestHelpers;

exception TestFailed string;

describe
  "Runner"
  [
    describe
      "example throws an exception"
      [
        it_w
          "returns an error message"
          (
            fun _ don => {
              let ex = anExampleGroup |> withExampleCode (fun _ => raise (TestFailed ""));
              run
                ex
                (
                  fun x =>
                    switch x {
                    | TestFailed => don ()
                    | TestSucceeded => don err::"Should fail" ()
                    }
                )
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
