open Dsl;

open TestHelpers;

open Matcher;

let exec x => ExampleGroup.empty |> applyOperation x |> run;

exception TestFailed string;

register (
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
                let x = TestContext.create ();
                x |> TestContext.add "x" 42;
                x |> TestContext.get "x" |> should (equal 42)
              }
            )
        ]
    ]
);

register (
  describe
    "Runner"
    [
      it
        "executes the example code"
        (
          fun _ => {
            let lines = ref [];
            let append line => lines := [line, ...!lines];
            anExampleGroup |> withExampleCode (fun _ => append "x") |> run;
            !lines |> should (equal ["x"])
          }
        )
    ]
);

Js.log "*  Starting test run";

!rootContext |> run;
