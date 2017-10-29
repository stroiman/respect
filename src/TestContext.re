module ContextMap = {
  include Map.Make(String);
  let merge = (a, b) =>
    merge(
      (_, x, y) =>
        switch x {
        | None => y
        | _ => x
        },
      a,
      b
    );
};

type contextMap = ContextMap.t(Obj.t);

type t = {mutable data: contextMap};

let add = (key, x, t) => t.data = t.data |> ContextMap.add(key, Obj.repr(x));

let get = (key, t) => t.data |> ContextMap.find(key) |> Obj.obj;

let create = () => {data: ContextMap.empty};
