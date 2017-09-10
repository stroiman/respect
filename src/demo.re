open Dsl;

let (<|) fn x => fn x;

register (describe "Specs" [
  it "works" <| fun () => {
    Js.log "Function 1";
  },
  it "works too" <| fun () => {
    Js.log "Function 2";
  },
]);

register (
describe "More specs" [
  it "works more" <| fun () => {
    Js.log "Function 3";
  },

  it "Works event more" <| fun () => {
    Js.log "Function 4";
  },
]);

let () = {
  Js.log "*  Starting test run";
  (!rootContext).children |> List.map (x => x.tests) |> List.flatten |> List.iter (fun (x:test) => {
    Js.log ("Running " ^ x.name);
    x.func();
  });
}
