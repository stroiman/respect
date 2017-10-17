let runRoot () => {
  let result = Respect.Runner.runRoot ();
  if (result |> Respect.TestResult.isSuccess) {
    Js.log "Test run succeeded";
    0
  } else {
    Js.log "Test run failed";
    1
  }
};
