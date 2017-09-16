module ContextMap = Map.Make String;

type contextMap = ContextMap.t Obj.t;

type t = {mutable data: contextMap};

let add key x t => t.data = t.data |> ContextMap.add key (Obj.repr x);

let get key t => t.data |> ContextMap.find key |> Obj.obj;

let create () => {data: ContextMap.empty};
