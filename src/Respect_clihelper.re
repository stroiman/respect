open Respect.Runner;

let runRoot = callback =>
  runRoot()
  |> Async.run(
       (result: RunResult.t) => {
         Js.logMany([|
           result |> RunResult.getNoOfPassedTests |> Js.Int.toString,
           "passed tests,",
           result |> RunResult.getNoOfPendingTests |> Js.Int.toString,
           "pending tests, ",
           result |> RunResult.getNoOfFailedTests |> Js.Int.toString,
           "failed tests",
         |]);
         if (result |> Respect.TestResult.isSuccess) {
           Js.log("Test run succeeded");
           callback(0);
         } else {
           Js.log("Test run failed");
           callback(1);
         };
       },
       ~fe=
         _ => {
           Js.log("Test run failed");
           callback(1);
         },
     );
