type matchResult('t) =
  | MatchSuccess('t)
  | MatchFailure(Obj.t);

exception MatchFailedException(string);

type matcher('a, 'b) = 'a => (matchResult('b) => unit) => unit;

let (>=>) = (a: matcher('a, 'b), b: matcher('b, 'c)) => (x: 'a) => (cb) =>
  a(x, (fun
      | MatchFailure(x) => cb(MatchFailure(x))
      | MatchSuccess(x) => b(x, cb)
      ));

let matchSuccess = (a) => cb => cb(MatchSuccess(a));
let matchFailure = (a) => cb => cb(MatchFailure(a |> Obj.repr));

let equal = (expected, actual) =>
  actual == expected ? matchSuccess(actual) : matchFailure(expected);

let beGreaterThan = (expected, actual) =>
  actual > expected ? matchSuccess(actual) : matchFailure(expected);

let beLessThan = (expected, actual) =>
  actual < expected ? matchSuccess(actual) : matchFailure(expected);

let should = (matcher: matcher('a, 'b), actual: 'a) => {
  let result = ref(None);
  matcher(actual)(r => result := Some(r));
  switch(result^) {
    | Some(MatchSuccess(_)) => ()
    | Some(MatchFailure(e)) => 
      Js.log("Match failed");
      Js.log2("Expected: ", e);
      Js.log2("Actual: ", actual);
      MatchFailedException("Match failed") |> raise
    | None => failwith("Matcher did not eval synchronously");
    }
  };

let shoulda = (matcher, actual, don: Respect_callbacks.doneCallback) => {
  let handleMatch = (result) =>
    switch result {
    | MatchSuccess(_) => don()
    | MatchFailure(expected) =>
      Js.log("Match failed");
      Js.log(("Expected: ", expected));
      Js.log(("Actual: ", actual));
      don(~err="match failed", ())
    };
  (matcher(actual))(handleMatch)
};
