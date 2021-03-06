module Matcher = Respect_matcher;
module MatchersV2 = Respect_matchersV2;
module Callbacks = Respect_callbacks;
module Ctx = Respect_ctx;
module Domain = Respect_domain;
module Dsl = Respect_dsl;

module AsyncInfix = {
  open Async;

  let (>>=) = (x, f) => x |> bind(f);
};

module Runner = {
  open Domain;

  let mergeResult = (a, b) =>
    switch (a, b) {
    | (TestFailed, _) => TestFailed
    | (_, TestFailed) => TestFailed
    | (TestPending, _) => TestPending
    | (_, TestPending) => TestPending
    | (TestSucceeded, TestSucceeded) => TestSucceeded
    };

  type runResult = {
    noOfPassed: int,
    noOfPending: int,
    noOfFailed: int,
  };

  module RunResult = {
    type t = runResult;
    let empty = {noOfPassed: 0, noOfPending: 0, noOfFailed: 0};
    let recordResult = (result, carry) =>
      switch (result) {
      | TestSucceeded => {...carry, noOfPassed: carry.noOfPassed + 1}
      | TestPending => {...carry, noOfPending: carry.noOfPending + 1}
      | TestFailed => {...carry, noOfFailed: carry.noOfFailed + 1}
      };
    let merge = (a, b) => {
      noOfPassed: a.noOfPassed + b.noOfPassed,
      noOfPending: a.noOfPending + b.noOfPending,
      noOfFailed: a.noOfFailed + b.noOfFailed,
    };
    let getNoOfPassedTests = x => x.noOfPassed;
    let getNoOfPendingTests = x => x.noOfPending;
    let getNoOfFailedTests = x => x.noOfFailed;
    let getResult = x =>
      x.noOfFailed > 0 ?
        TestFailed : x.noOfPending > 0 ? TestPending : TestSucceeded;
  };

  let runExample = (groupStack, ex: example): Async.t(executionResult) => {
    let ctx = {
      let mdStack = groupStack |> List.map(x => x.metadata);
      let mdStack' = [ex.metadata, ...mdStack];
      let md =
        List.fold_left(Ctx.ContextMap.merge, Ctx.ContextMap.empty, mdStack');
      Ctx.create(md);
    };

    let logError = r => {
      if (r == TestPending) {
        let groupNames =
          List.fold_left(
            (acc, grp) =>
              if (grp.name == "") {
                acc;
              } else {
                grp.name ++ " - " ++ acc;
              },
            "",
            groupStack,
          );
        Js.log("EXAMPLE: " ++ groupNames ++ ex.name ++ " - PENDING");
      };
      if (r == TestFailed) {
        let groupNames =
          List.fold_left(
            (acc, grp) =>
              if (grp.name == "") {
                acc;
              } else {
                grp.name ++ " - " ++ acc;
              },
            "",
            groupStack,
          );
        Js.log("EXAMPLE: " ++ groupNames ++ ex.name ++ " - FAILED");
      };
      r;
    };

    let doRun = () => ex.func(ctx) |> Async.from_callback;

    let rec runParentGroups = grps: Async.t(executionResult) =>
      switch (grps) {
      | [] => doRun()
      | [grp, ...parents] =>
        let rec runSetups = setups: Async.t(executionResult) =>
          switch (setups) {
          | [] => runParentGroups(parents)
          | [Setup(x), ...rest] =>
            x(ctx)
            |> Async.from_callback
            |> Async.bind(
                 fun
                 | TestFailed => Async.return(TestFailed)
                 | TestPending => Async.return(TestPending)
                 | TestSucceeded => runSetups(rest),
               )
          };
        runSetups(grp.setups);
      };
    runParentGroups(groupStack |> List.rev)
    |> Async.timeout(Async.Seconds(1))
    |> Async.tryCatch(_ => Some(TestFailed))
    |> Async.map(logError);
  };
  let rec run = (grp, filter, parents): Async.t(runResult) => {
    open AsyncInfix;
    let groupStack = [grp, ...parents];
    let rec iter = (state: RunResult.t, tests): Async.t(runResult) =>
      switch (tests |> List.filter(filter)) {
      | [] => Async.return(state)
      | [ex, ...rest] =>
        runExample(groupStack, ex)
        >>= (result => iter(RunResult.recordResult(result, state), rest))
      };
    let rec iterGrps = (state, grps): Async.t(runResult) =>
      switch (grps) {
      | [] => Async.return(state)
      | [grp, ...rest] =>
        run(grp, filter, groupStack)
        >>= (result => iterGrps(RunResult.merge(result, state), rest))
      };
    iter(RunResult.empty, grp.examples)
    >>= (exampleResults => iterGrps(exampleResults, grp.children));
  };
  /* Runs all tests in a single example group. Since a group has no knowledge
     of its parents, using this function will not run setup code registered in
     parents */
  let run = grp: Async.t(runResult) => {
    let filter =
      ExampleGroup.hasFocusedExamples(grp) ?
        x => Example.isFocused(x) && !Example.isSkipped(x) :
        (x => !Example.isSkipped(x));
    run(grp, filter, []);
  };
  /* Runs all tests registered in the root example group */
  let runRoot = (): Async.t(runResult) => run(Dsl.rootContext^);
};

module TestResult = {
  open Runner;
  let isSuccess = result => result.noOfFailed > 0 ? false : true;
};
