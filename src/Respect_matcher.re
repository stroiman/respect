type syncMatchResult 't =
  | MatchSuccess 't
  | MatchFailure Obj.t;

type asyncMatchResult 'a = (syncMatchResult 'a => unit) => unit;

exception MatchFailedException string;

type matchResult 'a =
  | SyncMatchResult (syncMatchResult 'a)
  | AsyncMatchResult (asyncMatchResult 'a);

let equal expected actual => {
  let result = actual == expected ? MatchSuccess actual : MatchFailure (Obj.repr expected);
  SyncMatchResult result
};

let should (matcher: 'a => matchResult 'b) (actual: 'a) => {
switch (matcher actual) {
  | AsyncMatchResult _ => raise (MatchFailedException "Cannot run async matcher synchronousely")
  | SyncMatchResult result => {
    switch result {
      | MatchSuccess _ => ()
      | MatchFailure expected =>
      Js.log "Match failed";
      Js.log ("Expected: ", expected);
      Js.log ("Actual: ", actual);
      MatchFailedException "Match failed" |> raise
      }}
  };
};

let shoulda matcher actual (don: Respect_callbacks.doneCallback) => {
  let handleMatch result =>
    switch result {
    | MatchSuccess _ => don ()
    | MatchFailure expected =>
      Js.log "Match failed";
      Js.log ("Expected: ", expected);
      Js.log ("Actual: ", actual);
      don err::"match failed" ()
    };

  switch (matcher actual) {
  | SyncMatchResult result => handleMatch result
  | AsyncMatchResult fn => fn handleMatch
  }
};
