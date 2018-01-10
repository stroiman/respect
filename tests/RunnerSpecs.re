module As = Respect_async;
module Ctx = Respect.Ctx;
open Respect.Dsl.Async;
open Respect.Domain;
/*open Respect.Runner;*/
open Respect.Matcher;
open TestHelpers;
open TestHelpers.AsyncMatchers;
exception MockFailure(string);

let run = Respect.Runner.run;
let runAndReturn = (res,examples) => run(examples) |> Async.map(~f=(_) => res^);

let beSuccess = actual => {
  open Respect.Runner;
  switch(actual.testResult) {
    | TestSucceeded => matchSuccess(actual)
    | _ => matchFailure(actual, actual)
  };
};

let beFailure = actual => {
  open Respect.Runner;
  switch(actual.testResult) {
    | TestFailed => matchSuccess(actual)
    | _ => matchFailure(actual, actual)
  };
};

let bePending = actual => {
  open Respect.Runner;
  switch(actual.testResult) {
    | TestPending => matchSuccess(actual)
    | _ => matchFailure(actual, actual)
  };
};

describe("Runner", [
  describe("Test Result", [
    it("Is success when all tests succeed", (_) => {
      let ex = anExampleGroup
        |> withExample( ~code = passingExample())
        |> withExample( ~code = passingExample());
      run(ex) |> shoulda(asyncResolve >=> beSuccess)
    }),
    it("Is pending when one test is pending", (_) => {
      let ex = anExampleGroup
        |> withExample( ~code = passingExample() )
        |> withExample( ~code = pendingExample() );
      run(ex) |> shoulda(asyncResolve >=> bePending)
    }),
    it("Is a failure when one test is pending", (_) => {
      let ex = anExampleGroup
        |> withExample( ~code = passingExample() )
        |> withExample( ~code = pendingExample() )
        |> withExample( ~code = failingExample() );
      run(ex) |> shoulda(asyncResolve >=> beFailure)
    })
  ]),

  describe("Group has a setup", [
    it("Doesn't execute example code when setup code fails", (_) => {
      let lines = ref([]);
      let append = (line) => lines := lines^ @ [line];
      let ex =
        anExampleGroup
          |> withSetup( (_, cb) => { append("setup"); cb(TestFailed) })
          |> withExample( ~code= (_, cb) => { append("test"); cb(TestSucceeded) });
      ex |> runAndReturn(lines)
      |> shoulda(asyncResolve >=> equal(["setup"]))
    }),

    it("Executes multiple setups before the example", (_) => {
      let lines = ref([]);
      let append = (line) => (_) => lines := lines^ @ [line];
      let ex =
        anExampleGroup
          |> withSetup(passingSetup(~onRun=append("setup 1"),()) )
          |> withSetup(passingSetup(~onRun=append("setup 2"),()) )
          |> withExample(~code=passingExample(~onRun=append("test"),()) );
      let expected = ["setup 1", "setup 2", "test"];
      ex |> runAndReturn(lines)
      |> shoulda(asyncResolve >=> equal(expected))
    }),

    it("Executes the setup code before the example", (_) => {
      let lines = ref([]);
      let append = (line) => (_) => lines := lines^ @ [line];
      let ex =
        anExampleGroup
          |> withSetup(passingSetup(~onRun=append("setup"),()))
          |> withExample(~code=passingExample(~onRun=append("test"),()));
      ex |> runAndReturn(lines)
      |> shoulda(asyncResolve >=> equal(["setup", "test"]))
    }),

    describe("Group has two examples", [
      it("Runs the setup before each example", (_) => {
        let lines = ref([]);
        let append = (line) => (_) => lines := lines^ @ [line];
        let ex =
          anExampleGroup
            |> withSetup(passingSetup(~onRun=append("setup"),()))
            |> withExample( ~code=passingExample(~onRun=append("test 1"),()))
            |> withExample( ~code=passingExample(~onRun=append("test 2"),()));
        let expected = ["setup", "test 1", "setup", "test 2"];
        ex |> runAndReturn(lines)
        |> shoulda(asyncResolve >=> equal(expected))
      })
    ]),

    describe("Nested groups", [
      it("Runs the setups from outermost to innermost group", (_) => {
        let lines = ref([]);
        let append = (line) => (_) => lines := lines^ @ [line];
        let innerGroup =
          anExampleGroup
            |> withSetup(passingSetup(~onRun=append("inner setup"),()))
            |> withExample(~code=passingExample(~onRun=append("inner test"),()));
        let outerGroup =
          anExampleGroup
            |> withSetup(passingSetup(~onRun=append("outer setup"),()))
            |> withExample(~code=passingExample(~onRun=append("outer test"),()))
            |> withChildGroup(innerGroup);
        let expected = [
          "outer setup", "outer test", "outer setup",
          "inner setup", "inner test"
        ];
        outerGroup |> runAndReturn(lines)
        |> shoulda(asyncResolve >=> equal(expected))
      })
    ])
  ]),

  describe("ExampleGroup has metadata", [
    it("Initializes the metadata on the test context", (_) => {
        let data = ref("");
        let grp =
          anExampleGroup
            |> withMetadata(("data", "value"))
            |> withExample(~code=passingExample(
              ~onRun=ctx => data := ctx |> Ctx.get("data"),()));
        grp |> runAndReturn(data)
        |> shoulda(asyncResolve >=> equal("value"))
    })
  ]),

  describe("Parent group has metadata", [
    it("uses the metadata closest to the example", (_) => {
      let lines = ref([]);
      let append = (line) => lines := lines^ @ [line];
      let innerGroup =
        anExampleGroup
          |> withMetadata(("data2", "inner"))
          |> withMetadata(("data3", "inner"))
          |> withExample(
            ~metadata=("data3", "test"),
            ~code=passingExample(~onRun=ctx => {
              append(ctx |> Ctx.get("data1"));
              append(ctx |> Ctx.get("data2"));
              append(ctx |> Ctx.get("data3"));
            },()));
      let outerGroup =
        anExampleGroup
          |> withMetadata(("data1", "outer"))
          |> withMetadata(("data2", "outer"))
          |> withMetadata(("data3", "outer"))
          |> withChildGroup(innerGroup);
      outerGroup |> runAndReturn(lines)
      |> shoulda(asyncResolve >=> equal(["outer", "inner", "test"]))
    })
  ]),

  describe("example throws an exception", [
    it("returns an error message", (_) => {
      let ex = anExampleGroup |> withExampleCode((_) => raise(MockFailure("")));
      run(ex) |> shoulda(asyncResolve >=> beFailure)
    })
  ]),

  it("executes the example code", (_) => {
    let lines = ref([]);
    let append = (line) => lines := [line, ...lines^];
    let ex = anExampleGroup |> withExampleCode((_) => append("x"));
    ex |> runAndReturn(lines)
    |> shoulda(asyncResolve >=> equal(["x"]))
  })
]) |> register;
