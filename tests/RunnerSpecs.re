module As = Respect_async;
open Respect.Dsl.Async;
open Respect.Domain;
open Respect.Runner;
open Respect.Matcher;
open TestHelpers;
exception TestFailed(string);

let run = (ex, cb) => run(ex) |> As.run(cb);

describe("Runner", [
  describe("Group has a setup", [
    it("Doesn't execute example code when setup code fails", (_, don) => {
      let lines = ref([]);
      let append = (line) => lines := lines^ @ [line];
      let ex =
        anExampleGroup
          |> withSetup( (_, cb) => { append("setup"); cb(TestFailed) })
          |> withExample( ~code= (_, cb) => { append("test"); cb(TestSucceeded) });
          run(ex, (_) => (lines^ |> shoulda(equal(["setup"])))(don))
          }
    ),
    it("Executes multiple setups before the example", (_, don) => {
      let lines = ref([]);
      let append = (line) => lines := lines^ @ [line];
      let ex =
        anExampleGroup
          |> withSetup( (_, cb) => { append("setup 1"); cb(TestSucceeded) })
          |> withSetup( (_, cb) => { append("setup 2"); cb(TestSucceeded) })
          |> withExample( ~code= (_, cb) => { append("test"); cb(TestSucceeded) });
          run(ex, (_) => (lines^ |> shoulda(equal(["setup 1", "setup 2", "test"])))(don))
          }
    ),
    it("Executes the setup code before the example", (_, don) => {
      let lines = ref([]);
      let append = (line) => lines := lines^ @ [line];
      let ex =
        anExampleGroup
          |> withSetup(
            (_, cb) => {
              append("setup");
              cb(TestSucceeded)
            }
          )
          |> withExample(
            ~code=
            (_, cb) => {
              append("test");
              cb(TestSucceeded)
            }
          );
      run(ex, (_) => (lines^ |> shoulda(equal(["setup", "test"])))(don))
      }
    ),
    describe("Group has two examples", [
      it("Runs the setup before each example", (_, don) => {
        let lines = ref([]);
        let append = (line) => lines := lines^ @ [line];
        let ex =
          anExampleGroup
            |> withSetup(
              (_, cb) => {
                append("setup");
                cb(TestSucceeded)
              }
            )
            |> withExample(
              ~code=
              (_, cb) => {
                append("test 1");
                cb(TestSucceeded)
              }
            )
            |> withExample(
              ~code=
              (_, cb) => {
                append("test 2");
                cb(TestSucceeded)
              }
            );
        run(
          ex,
          (_) => (lines^ |> shoulda(equal(["setup", "test 1", "setup", "test 2"])))(don)
          )
      }
    )]
  ),
  describe("Nested groups", [
    it("Runs the setups from outermost to innermost group", (_, don) => {
      let lines = ref([]);
      let append = (line) => lines := lines^ @ [line];
      let innerGroup =
        anExampleGroup
          |> withSetup(
            (_, cb) => {
              append("inner setup");
              cb(TestSucceeded)
            }
          )
          |> withExample(
            ~code=
            (_, cb) => {
              append("inner test");
              cb(TestSucceeded)
            }
          );
      let outerGroup =
        anExampleGroup
          |> withSetup(
            (_, cb) => {
              append("outer setup");
              cb(TestSucceeded)
            }
          )
          |> withExample(
            ~code=
            (_, cb) => {
              append("outer test");
              cb(TestSucceeded)
            }
          )
          |> withChildGroup(innerGroup);
          run(
            outerGroup,
            (_) => {
              let expected = [
                "outer setup",
                  "outer test",
                  "outer setup",
                  "inner setup",
                  "inner test"
              ];
              (lines^ |> shoulda(equal(expected)))(don)
              }
          )
        }
      )]
    )]
  ),
  describe("ExampleGroup has metadata", [
    it("Initializes the metadata on the test context",
      (_, don) => {
        let lines = ref([]);
        let append = (line) => lines := lines^ @ [line];
        let grp =
          anExampleGroup
            |> withMetadata(("data", "value"))
            |> withExample(
              ~code=
              (ctx, cb) => {
                append(ctx |> TestContext.get("data"));
                cb(TestSucceeded)
              }
            );
        run(
          grp,
          (_) => {
            let expected = ["value"];
            (lines^ |> shoulda(equal(expected)))(don)
            }
        )
      }
  )]
  ),
  describe("Parent group has metadata", [
    it("uses the metadata closest to the example", (_, don) => {
        let lines = ref([]);
        let append = (line) => lines := lines^ @ [line];
        let innerGroup =
          anExampleGroup
            |> withMetadata(("data2", "inner"))
            |> withMetadata(("data3", "inner"))
            |> withExample(
              ~metadata=("data3", "test"),
              ~code=
              (ctx, cb) => {
                append(ctx |> TestContext.get("data1"));
                append(ctx |> TestContext.get("data2"));
                append(ctx |> TestContext.get("data3"));
                cb(TestSucceeded)
              }
            );
        let outerGroup =
          anExampleGroup
            |> withMetadata(("data1", "outer"))
            |> withMetadata(("data2", "outer"))
            |> withMetadata(("data3", "outer"))
            |> withChildGroup(innerGroup);
            run(
              outerGroup,
              (_) => {
                let expected = ["outer", "inner", "test"];
                (lines^ |> shoulda(equal(expected)))(don)
                }
            )
      })
]),
  describe("example throws an exception", [
    it("returns an error message", (_, don) => {
      let ex = anExampleGroup |> withExampleCode((_) => raise(TestFailed("")));
      run(
        ex,
        fun
        | TestFailed => don()
        | TestSucceeded 
        | TestPending => don(~err="Should fail", ())
        )
    }
    )
  ]
  ),
  it(
    "executes the example code",
    (_) => {
      let lines = ref([]);
      let append = (line) => lines := [line, ...lines^];
      let ex = anExampleGroup |> withExampleCode((_) => append("x"));
      run(ex, ignore);
      lines^ |> shoulda(equal(["x"]))
      }
  )
  ])
  |> register;
