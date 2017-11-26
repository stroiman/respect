open Respect_callbacks;

module ContextMap = {
  include Map.Make(String);
  let merge = (a, b) => merge( (_, x, y) => switch x { | None => y | _ => x }, a, b);
};

type contextMap = ContextMap.t(Obj.t);

type t = {.
  add: 'a. string => 'a => t,
  get: 'a. string => 'a,
  setSubj: 'a. (t => 'a) => t,
  subject: 'a. unit => 'a,
  don: unit => doneCallback => unit
};

let create = (metaData) : t => {
  val mutable data = metaData;
  val mutable subjFn : option(t => Obj.t)= None;
  val mutable subj : option(Obj.t) = None;

  pub add : 'a. string => 'a => t = (key,x)  => { 
    data = data |> ContextMap.add(key, x |> Obj.repr); 
    this 
  };

  pub get : 'a. string => 'a = key => { 
    data |> ContextMap.find(key) |> Obj.obj
    };

  pub setSubj: 'a. (t => 'a) => t = fn => { 
    subjFn = Some(x => fn(x) |> Obj.repr); 
    this 
  };

  pub subject : 'a. unit => 'a = () =>
    switch(subj) {
      | None => {
        let s = Js.Option.getExn(subjFn)(this);
        subj = Some(s);
        s |> Obj.obj
        }
      | Some(x) => x |> Obj.obj
      };

  pub don = () => d => d();
};

let add = (key : string, x : 'a, ctx : t) => ctx#add(key, x);
let get = (key : string, ctx : t) : 'a => ctx#get(key);
let setSubj = (fn : t => 'a, ctx: t) => ctx#setSubj(fn);
let subject = (ctx:t) => ctx#subject();

let map = (key : string, f: 'a => 'b, ctx : t) => {
  let updated = ctx |> get(key) |> f;
  ctx |> add(key, updated);
};

let don = (ctx:t) : (doneCallback => unit) => ctx#don();


