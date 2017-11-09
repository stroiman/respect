type t('a) = (('a => unit, exn => unit)) => unit;

let return = (x: 'a) : t('a) => ((cb, _)) => cb(x);

exception JsError(Js.Exn.t);

let bind = (~f: 'a => t('b), x: t('a)) : t('b) =>
  ((successCb, errorCB)) =>
    x((
      (a) =>
        try (f(a, (successCb, errorCB))) {
        | x => errorCB(x)
        },
      errorCB
    ));

let map = (~f: 'a => 'b, x: t('a)) : t('b) =>
  ((successCb, errorCb)) =>
    x((
      (a) =>
        switch (f(a)) {
        | x => successCb(x)
        | exception x => errorCb(x)
        },
      errorCb
    ));

let from_js = (jsAsync: ((Js.Null.t(Js.Exn.t), 'a) => unit) => unit) : t('a) =>
  ((successCb, errorCb)) =>
    jsAsync(
      (err, s) =>
        switch (Js.Null.to_opt(err)) {
        | None => successCb(s)
        | Some(x) => errorCb(JsError(x))
        }
    );

let from_callback = (fn: ('a => unit) => unit) : t('a) =>
  ((successCb, _)) => fn(successCb);

let run = (f, x) => x((f, (_) => ()));

let runExn = (~fs, ~fe, x) => x((fs, fe));
