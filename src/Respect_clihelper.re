let runRoot = (callback) =>
  Respect.Runner.runRoot ()
  |> Async.runExn(~fs=
    (result) =>
      if (result |> Respect.TestResult.isSuccess) {
        Js.log("Test run succeeded");
        callback(0)
      } else {
        Js.log("Test run failed");
        callback(1)
      },
      ~fe=(_) => {
        Js.log("Test run failed");
        callback(1)
      }
  );
