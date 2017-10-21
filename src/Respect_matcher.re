type matchResult 't =
  | MatchSuccess 't
  | MatchFailure Obj.t;

exception MatchFailedException string;

let equal expected actual =>
  actual == expected ? MatchSuccess actual : MatchFailure (Obj.repr expected);

let should (matcher: 'a => matchResult 'b) (actual: 'a) =>
  switch (matcher actual) {
  | MatchSuccess _ => ()
  | MatchFailure expected =>
    Js.log "Match failed";
    Js.log ("Expected: ", expected);
    Js.log ("Actual: ", actual);
    MatchFailedException "Match failed" |> raise
  };
