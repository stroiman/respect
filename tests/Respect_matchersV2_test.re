open Respect.Dsl.Async;
open Respect.MatchersV2;
open Respect_callbacks;

let shouldMatch = (fn:doneCallback => unit) => (don:doneCallback) => {
  fn(fun (~err=?,()) => {
    switch(err) {
      | None => don();
      | Some(x) => don(~err=x,());
    }
  })
};

let shouldNotMatch = (fn:doneCallback => unit) => (don:doneCallback) => {
  fn(fun (~err=?,()) => {
    switch(err) {
      | None => don(~err="Expected match error, but none was received",());
      | Some(_) => don();
    }
  })
};

describe("MatchersV2", [
  describe("equal", [
    it("matches when args are equal", (_) => {
      expect(5)#to_(equal(5))
        |> shouldMatch
    }),

    it("fails when args are not equal", (_) => {
      expect(5)#to_(equal(6))
        |> shouldNotMatch
    })
  ])
]) |> register
