type syncMatchResult 't =
  | MatchSuccess 't
  | MatchFailure Obj.t;

type asyncMatchResult 'a = (syncMatchResult 'a => unit) => unit;

exception MatchFailedException string;

type matchResult 'a =
  | SyncMatchResult (syncMatchResult 'a)
  | AsyncMatchResult (asyncMatchResult 'a);

type matcher 'a 'b = 'a => matchResult 'b;

let (>=>) (a : matcher 'a 'b) (b : matcher 'b 'c) => {
  let result : matcher 'a 'c = fun (x:'a) => {
    switch (a x) {
      | SyncMatchResult(MatchSuccess(x)) => b x
      | SyncMatchResult(MatchFailure(x)) => SyncMatchResult(MatchFailure(x))
      | AsyncMatchResult(x) => 
        AsyncMatchResult(fun cb => {
          x(fun firstResult => {
            switch firstResult {
              | MatchFailure x => cb (MatchFailure x)
              | MatchSuccess x => {
                switch (b x) {
                  | SyncMatchResult x => cb x
                  | AsyncMatchResult x => x cb
                }
              }
            }
          })
        })
    }
  };
  result;
};

let equal expected => fun actual =>
  SyncMatchResult (actual == expected 
                   ? MatchSuccess actual 
                   : MatchFailure (Obj.repr expected));


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
