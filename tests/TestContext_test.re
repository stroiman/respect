open Respect;
open Respect.Dsl.Async;
open Respect.Matcher;
open Respect.Domain;

let beFailure = (result) =>
  switch result {
  | TestFailed => MatchSuccess()
  | _ => MatchFailure(Obj.repr(), Obj.repr())
  };

let create = () => Ctx.create(Ctx.ContextMap.empty);

describe("TestContext", [
  describe("done helper for setup", [
    beforeEach(ctx => ctx |> Ctx.add("key",42) |> Ctx.don),

    it("Uses value from setup", (ctx) => {
      ctx
        |> Ctx.get("key")
        |> shoulda(equal(42))
    })
  ]),

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
    }),

    describe("Subject", [
      it("is not evaluated until used", (_) => {
        let ctx = create();
        let ctx = ctx |> Ctx.setSubj(ctx => ctx#get("key") + 1);
        let ctx = ctx#add("key", 42);
        Ctx.subject(ctx) |> shoulda(equal(43));
      })
    ])
  ]),
  describe("Update data", [
    it("modifies a key based on a function", (_) => {
      let ctx = create();
      ctx 
        |> Ctx.add("key", 42)
        |> Ctx.map("key", x => x+1)
        |> Ctx.get("key") |> shoulda(equal(43))
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
