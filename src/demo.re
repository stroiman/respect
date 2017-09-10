open Dsl;

let (<|) fn x => fn x;

register (
  describe
    "Specs"
    [
      it "works" <| (fun () => Js.log "Function 1"),
      it "works too" <| (fun () => Js.log "Function 2")
    ]
);

register (
  describe
    "More specs"
    [
      it "works more" <| (fun () => Js.log "Function 3"),
      it "Works event more" <| (fun () => Js.log "Function 4"),
      describe "Child specs" [it "Has child spec" <| (fun () => Js.log "Child spec")]
    ]
);

Js.log "*  Starting test run";

!rootContext |> run;
