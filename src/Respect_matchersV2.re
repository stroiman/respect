open Respect_callbacks;

type matchResult('a) =
  | MatchSuccess('a)
  | MatchFailure(Obj.t);

type async('a) = (('a => unit, exn => unit)) => unit;

module Async = {
  type t('a) = async('a);
  let return = (x:'a) : t('a) => ((cb,_)) => cb(x);
  let bind = (~f:'a=>t('b),~x:t('a)):t('b) => callbacks => {
    let errorCB = callbacks |> snd;
    switch( x((a => f(a)(callbacks),errorCB))) 
    {
      | () => ()
      | exception x => errorCB(x)
    }
  };
  let run = (f,x) => x((f,(_) => {()}));
  let runExn = (~fs,~fe,x) => (x((fs,fe)));
};

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
