/*
    Module Domain describes the internal structure that represents examples
    and groups of examples.
 */

/* Represents the outcome of running a test */
type executionResult =
  | TestPending
  | TestSucceeded
  | TestFailed;
type executionCallback = executionResult => unit;
/* Internal implementation, a nicer callback is exposed via the DSL */
type testFunc = (Respect_ctx.t, executionCallback) => unit;
type example = {
  name: string,
  func: testFunc,
  metadata: Respect_ctx.contextMap,
  focused: bool,
  skipped: bool,
};
type setup =
  | Setup(testFunc);
/* A group of examples, and nested groups */
type exampleGroup = {
  name: string,
  children: list(exampleGroup),
  setups: list(setup),
  examples: list(example),
  metadata: Respect_ctx.contextMap,
};
module ExampleGroup = {
  let empty = {
    name: "",
    children: [],
    setups: [],
    examples: [],
    metadata: Respect_ctx.ContextMap.empty,
  };
  let addChild = (child, root) => {
    ...root,
    children: root.children @ [child],
  };
  let addExample = (ex, grp) => {...grp, examples: grp.examples @ [ex]};
  let addSetup = (code, grp) => {...grp, setups: grp.setups @ [code]};

  let rec hasFocusedExamples = grp => {
    let tmp =
      List.fold_left(
        (acc, ex) => ex.focused ? true : acc,
        false,
        grp.examples,
      );
    List.fold_left(
      (acc, child) => hasFocusedExamples(child) ? true : acc,
      tmp,
      grp.children,
    );
  };
};

module Example = {
  let isFocused = x => x.focused;
  let isSkipped = x => x.skipped;
};
