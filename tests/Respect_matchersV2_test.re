open Respect.Dsl.Async;
open Respect.MatchersV2;
open TestHelpers.MatchHelper;

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
