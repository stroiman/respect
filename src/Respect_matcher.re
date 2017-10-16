type matchResult 't =
  | Success 't
  | Failure;

exception MatchFailedException string;

let equal expected actual => actual == expected ? Success actual : Failure;

let should matcher actual =>
  switch (matcher actual) {
  | Success _ => ()
  | Failure => MatchFailedException "Match failed" |> raise
  };
