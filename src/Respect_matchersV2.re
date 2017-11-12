open Respect_callbacks;

module Async = Respect_async;

type matchResult('a) =
  | MatchSuccess('a)
  | MatchFailure(Obj.t);

type async('a) = Async.t('a);

type matcher('a,'b) = {.
  f: 'a => async(matchResult('b))
};

let createCallback = (don:doneCallback) => 
  fun 
  | MatchSuccess(_) => don() 
  | MatchFailure(_) => don(~err="Match error",());

let expect = (actual) => {
  pri actual = actual; /* avoid warning */
  pub to_ = (matcher:matcher('a,'b)) => (don: doneCallback) => {
    matcher#f(this#actual)
      |> Async.runExn (
        ~fs=createCallback(don), 
        ~fe=(_) => don(~err="Exception occurred",()))
  }
};

let equal = (expected:'a) : matcher('a,'a) => {
  pri expected = expected;
  pub f = (actual) => {
    if (actual == this#expected) { 
      MatchSuccess(actual) |> Async.return
    } else {
      MatchFailure(actual |> Obj.repr)|> Async.return
    } 
  }
};
