let runRoot = (callback) =>
  Respect.Runner.runRoot(
    (result) =>
      if (result |> Respect.TestResult.isSuccess) {
        Js.log("Test run succeeded");
        callback(0)
      } else {
        Js.log("Test run failed");
        callback(1)
      }
  );
