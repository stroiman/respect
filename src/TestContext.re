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

type t = {.
  add: string => Obj.t => t,
  get: string => Obj.t
};

let add = (key, x, t) => t#add(key, Obj.repr(x));
/*let add = (key, x, t) => t.data = t.data |> ContextMap.add(key, Obj.repr(x));*/

let get = (key, t) => t#get(key) |> Obj.obj;
/*let get = (key, t) => t.data |> ContextMap.find(key) |> Obj.obj;*/

let create = (metaData) : t => {
  val data = ref(metaData);
  pub add = (key,x) => { data := data^ |> ContextMap.add(key, x); this };
  pub get = (key) => data^ |> ContextMap.find(key);
  }
