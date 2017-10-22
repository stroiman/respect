open Respect.Dsl;
open Respect.Domain;
open Respect.Runner;
open Respect.Matcher;
open TestHelpers;

exception TestFailed string;

describe "Runner" [
  describe "Group has a setup" [
    it_w "Doesn't execute example code when setup code fails" (fun _ don => {
      let lines = ref [];
      let append line => lines := !lines @ [line];
      let ex = anExampleGroup
        |> withSetup (fun _ cb => { append "setup"; cb TestFailed })
        |> withExample code::(fun _ cb => { append "test"; cb TestSucceeded });
      run ex (fun _ => {
        (!lines |> shoulda (equal ["setup"])) don
      });
    }),

    it_w "Executes multiple setups before the example" (fun _ don => {
      let lines = ref [];
      let append line => lines := !lines @ [line];
      let ex = anExampleGroup
        |> withSetup (fun _ cb => { append "setup 1"; cb TestSucceeded })
        |> withSetup (fun _ cb => { append "setup 2"; cb TestSucceeded })
        |> withExample code::(fun _ cb => { append "test"; cb TestSucceeded });
      run ex (fun _ => {
        (!lines |> shoulda (equal ["setup 1", "setup 2", "test"])) don;
      });
    }),

    it_w "Executes the setup code before the example" (fun _ don => {
      let lines = ref [];
      let append line => lines := !lines @ [line];
      let ex = anExampleGroup
        |> withSetup (fun _ cb => { append "setup"; cb TestSucceeded })
        |> withExample code::(fun _ cb => { append "test"; cb TestSucceeded });
      run ex (fun _ => {
        (!lines |> shoulda (equal ["setup", "test"])) don;
      });
    })
  ],
  describe "example throws an exception" [
    it_w "returns an error message" (
      fun _ don => {
        let ex = anExampleGroup |> withExampleCode (fun _ => raise (TestFailed ""));
        run ex (
          fun 
          | TestFailed => don ()
          | TestSucceeded => don err::"Should fail" ()
          )
      }
    )
  ],
  it "executes the example code" (fun _ => {
    let lines = ref [];
    let append line => lines := [line, ...!lines];
    let ex = anExampleGroup |> withExampleCode (fun _ => append "x");
    run ex ignore;
    !lines |> should (equal ["x"])
    }
  )
  ] |> register;
