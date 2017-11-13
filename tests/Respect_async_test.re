open Respect.Dsl.Async;
open Respect.Matcher;
open TestHelpers.AsyncMatchers;

exception Dummy;
exception Dummy2;

module Async = Respect_async;

[@bs.module "errorFunctions"]
external asyncThrowingFunction : (int, (Js.null(Js.Exn.t), int) => unit) => unit = "";
[@bs.module "errorFunctions"]
external asyncSucceedingFunction : (int, (Js.null(Js.Exn.t), int) => unit) => unit = "";

let haveMessage = expected => actual => 
  switch(actual) {
    | Async.JsError(err) => equal(expected, Js.Exn.message(err))
    | _ => SyncMatchResult(MatchFailure(actual |> Obj.repr))
    };

describe("Async module", [
  describe("return", [
    it("eventually returns the value", (_) => {
      Async.return(42) 
        |> shoulda(asyncResolve >=> equal(42))
    })
  ]),

  describe("map", [
    it("eventually returns the modified value", (_) => {
      Async.return(42) 
        |> Async.map(~f=x => x+1)
        |> shoulda(asyncResolve >=> equal(43))
    }),

    it("eventually returns error if mapping function throws", (_) => {
      Async.return(44) 
        |> Async.map(~f=(_) => {raise(Dummy);})
        |> shoulda(asyncThrow >=> equal(Dummy))
    })
  ]),

  describe("bind", [
    it("eventually returns the bound value", (_) => {
      Async.return(42) 
        |> Async.bind(~f=x => Async.return(x+1))
        |> shoulda(asyncResolve >=> equal(43))
    }),

    it("eventually returns error if bound function throws", (_) => {
      Async.return(42) 
        |> Async.bind(~f=(_) => { raise(Dummy) })
        |> shoulda(asyncThrow >=> equal(Dummy));
    })
  ]),

  describe("from_callback", [
    it("creates an async that succeeds", (_) => {
      let asyncFuncWithoutExn = (input, cb) => cb(input);
      asyncFuncWithoutExn(42)
        |> Async.from_callback
        |> shoulda(asyncResolve >=> equal(42))
    })
  ]),

  describe("Timeout", [
    it("fails when timeout exceeded", (_) => {
      let after = ((cb,_)) => Js.Global.setTimeout(() => cb(42), 5) |> ignore;
      after
        |> Async.timeout(MilliSeconds(1))
        |> shoulda(asyncThrow >=> equal(Async.Timeout));
    }),

    it("Succeeds when timeout not exceeded", (_) => {
      let after = ((cb,_)) => Js.Global.setTimeout(() => cb(42), 1) |> ignore;
      after
        |> Async.timeout(MilliSeconds(5))
        |> shoulda(asyncResolve >=> equal(42));
    })
  ]),

  describe("tryCatch", [
    it("async resolve Some value", (_) => {
      Async.return(43)
        |> Async.map(~f=(_) => { raise(Dummy2) })
        |> Async.tryCatch(~f=fun | Dummy => Some(42) | _ => None)
        |> shoulda(asyncThrow >=> equal(Dummy2))
    }),

    it("async throws None value", (_) => {
      Async.return(43)
        |> Async.map(~f=(_) => { raise(Dummy) })
        |> Async.tryCatch(~f=fun | Dummy => Some(42) | _ => None)
        |> shoulda(asyncResolve >=> equal(42))
    }),
  ]),

  describe("mapping from async js functions", [
    /* It is a common JS pattern to let functions take a callback
       that should accept two arguments, the first being an error, 
       which could be null, and the second being the result if no 
       error occurred */
    it("handles JS errors nicely", (_) => {
      asyncThrowingFunction(42)
        |> Async.from_js
        |> shoulda(asyncThrow >=> haveMessage(Some("dummy")));
    }),

    it("also handles when no error occues", (_) => {
      asyncSucceedingFunction(42)
        |> Async.from_js
        |> shoulda(asyncResolve >=> equal(42))
    })
  ])
]) |> register
