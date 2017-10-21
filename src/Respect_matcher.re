type matchResult 't =
  | MatchSuccess 't
  | MatchFailure;

exception MatchFailedException string;

let equal expected actual => actual == expected ? MatchSuccess actual : MatchFailure;

let should matcher actual =>
  switch (matcher actual) {
  | MatchSuccess _ => ()
  | MatchFailure =>
    let msg: string = Format.sprintf "Match failed Actual: %d " actual;
    MatchFailedException msg |> raise
  };
