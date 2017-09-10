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

let describe name tests => {
  name: name,
  children: [],
  setups: [],
  tests: tests
};

module TestContext = {
  let empty = { name : "", children : [], setups : [], tests : [] };
  let addChild child root => { ...root, children : root.children @ [child]}; 
};

let rootContext = ref TestContext.empty;
let register ctx => rootContext := !rootContext |> TestContext.addChild ctx;
