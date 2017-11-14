module ContextMap = {
  include Map.Make(String);
  let merge = (a, b) => merge( (_, x, y) => switch x { | None => y | _ => x }, a, b);
};

type contextMap = ContextMap.t(Obj.t);

type t = {.
  add: 'a. string => 'a => t,
  get: 'a. string => 'a,
};

let create = (metaData) : t => {
  val data = ref(metaData);
  pub add : 'a. string => 'a => t = (key,x)  => { 
    Js.log(("Adding key", key, x));
    data := data^ |> ContextMap.add(key, x |> Obj.repr); 
    this 
  };
  pub get : 'a. string => 'a = key => { 
    Js.log(("Getting entries", data^ |> ContextMap.bindings));
    data^ |> ContextMap.find(key) |> Obj.obj
  };
};

let add = (key : string, x : 'a, ctx : t) => ctx#add(key, x);
let get = (key : string, ctx : t) : 'a => ctx#get(key)
