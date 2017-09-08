type testFunc = unit => unit;

type test = {
  name: string,
  func: testFunc
};

type setup = Setup of testFunc;

type testContext = {
  name: string,
  children: list testContext,
  setups: list setup,
  tests: list test
};

let it name test => { name: name, func: test };

let (<|) fn x => fn x;

let x = it "works" <| fun () => {
  Js.log "Function!!";
};

let globalTests = ref [];

let addTest (t : test) => {
  globalTests := [t, ...!globalTests];
};

addTest x;

let () = {
  !globalTests |> List.iter (fun (x:test) => {
    Js.log ("Running " ^ x.name);
    x.func();
  });
}
