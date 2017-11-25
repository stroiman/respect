open Respect.Dsl.Async;
open Respect.MatchersV2;
open TestHelpers.MatchHelper;

module Int = { 
  type t = int; 
  let to_string = Js.Int.toString; 
  let compare = (a,b) => Int64.compare(Int64.of_int(a), Int64.of_int(b));
};

module String = {
  include String;
  let to_string = x => x;
};

module IntMatchers = Make(Int);
module StrMatchers = Make(String);

describe("MatchersV2", [
  describe("equal", [
    it("matches when args are equal", (_) => {
      expect(5).to_(equal(5))
        |> shouldMatch
    }),

    it("fails when args are not equal", (_) => {
      expect(5).to_(IntMatchers.equal(6))
        |> shouldNotMatch
    }),
  ]),

  describe("Description", [
    it("is nice", (_) => {
      let matcher = IntMatchers.equal(42);
      expect(matcher.description).to_(StrMatchers.equal("equal to 42"))
    })
  ])
]) |> register
