open Respect.Dsl.Async;
open Respect.Matcher;
open Respect.Domain;

module Ctx = TestContext;

let beFailure = (result) =>
  switch result {
  | TestFailed => MatchSuccess()
  | _ => MatchFailure(Obj.repr(), Obj.repr())
  };

let create = () => TestContext.create(TestContext.ContextMap.empty);

describe("TestContext", [
  describe("Get/set data to context", [
    it("Data added to context is retrievable", (_) => {
      let ctx = create();
      ctx
        #add("key", 42)
        #get("key") |> shoulda(equal(42));
    }),
    it("Data new values overwrite old values", (_) => {
      let ctx = create();
      let ctx = ctx#add("key2", 42);
      ctx#add("key", 43)#get("key") |> shoulda(equal(43))
    }),
    it("Subjec to change!!! Context is mutated", (_) => {
      let ctx = create();
      ctx#add("key", 42)
      #get("key") |> shoulda(equal(42));
    })
  ]),
  describe("Access with piping", [
    it("Can be piped", (_) => 
      create()
        |> Ctx.add("key", 42)
        |> Ctx.add("key", 43)
        |> Ctx.get("key")
        |> shoulda(equal(43))
    )
  ])
]) |> register;
