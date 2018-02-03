module Ctx = Respect_ctx;

/* Represents the outcome of running a test */
type executionResult =
| TestPending
| TestSucceeded
| TestFailed;
type executionCallback = executionResult => unit;
/* Internal implementation, a nicer callback is exposed via the DSL */
type testFunc = (Ctx.t, executionCallback) => unit;
type example = {
  name: string,
  func: testFunc,
  metadata: Ctx.contextMap
};
type setup =
| Setup(testFunc);
/* A group of examples, and nested groups */
type exampleGroup = {
  name: string,
  children: list(exampleGroup),
  setups: list(setup),
  examples: list(example),
  metadata: Ctx.contextMap
};
module ExampleGroup = {
  let empty = {name: "", children: [], setups: [], examples: [], metadata: Ctx.ContextMap.empty};
  let addChild = (child, root) => {...root, children: root.children @ [child]};
  let addExample = (ex, grp) => {...grp, examples: grp.examples @ [ex]};
  let addSetup = (code, grp) => {...grp, setups: grp.setups @ [code]};
};

