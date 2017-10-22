# ReSpect

This is an RSpec inspired test framework for ReasonML/OCaml/Bucklescript. The
runner uses raw javascript code, so it will only run in node environments at the moment.

I base this on a lot of experience I gained from a similar project for F#, FSpec

This project is still in a very earyly stage, so use at your own risk.

Although, I already did write an ML-ish functional test framework, some problem
solutions do not apply in a Reason/Bucklescript context - in particular testing
async code.

## TODO

 * "Finalize" DSL for building test suites.
 * Determine whether or not to allow mutation of `TestContext`.
 * Finalize assertion framework.
 * Nicer test output when running.
 * Nicer test output when assertions fail{...context, setups: e.
 * Determine how to best store per-test data. Mutable context worked well in
     FSpect, but we don't have runtime type checking in Reason/Bucklescript.

Although, I had learned from many mistakes when building FSpec, there are some
problems that demand different solutions in Reason/Bucklescript. Async support
in particular.

## Installation

This guide will help you get `Respect` and having a test-watcher setup.
_Respect_ in itself does not implement test-watcher functionality, but it is
easily added with the _nodemon_ package.

### Basic installation

First, add _respect_, the npm package is named "re-respect"

```
npm install --save-dev re-respect
```

As this is a package with Reason code, you need to add a reference to the
package in the _bsconfig.json_ file, as well.

You also need to add a _tests_ folder to contain the tests. At this early time
of writing, the only place that Respect searches for tests files is in the
_tests_ folder, so be sure that is the exact name of the folder.

```
"files": [
  {"dir": "src"},
  {"dir": "tests",
   "type": "dev" }
],
"bs-dev-dependencies": [
  "re-respect"
]
```

Create a skeleton test, "./tests/tests.re":

```
open Respect.Dsl;

describe "My first test" [
  it "runs" (fun _ => {()})
] |> register
```

The `register` call is necessary at this early stage, it adds the specs to one
global test group.

Now, let's add a test target to _package.json_

```
"scripts": {
   ...
   "test": "respect"
}
```

And now, you can run the tests with `npm run test`

### Adding test watcher functionality

The npm package _nodemon_ can trigger running _.js_ files when the file system
changes. We can use this to implement filesystem watcher functionality. First
install the package

```
npm install --save-dev nodemon
```

And then add a script to the _package.json_ file

```
  "scripts": {
    ...
    "test:watch": "nodemon node_modules/re-respect/bin/respect"
  }
```

And now, you can have the tests run automatically when a _.js_ file changes
with the command `npm run test:watch`. Of course, when you edit reason source
files, that will not trigger a test run, so you need to run `npm run watch` in a
different terminal.

### Optionally, create a _dev_ task

In the previous section, you had to run two watchers in two separate terminals
in order to have full watcher implementation. We can create an npm script that
does both of these tasks with the help of the npm package _npm-run-all_, which
allows parallel execution of multiple scripts.

```
npm install --save-dev npm-run-all
```

In the _package.json_ file, add a new script:

```
  "scripts": {
    ...
    "dev": "run-p watch test:watch"
  }
```

The command `run-p` is part of _npm-run-all_, and it runs the two scripts in
parallel.

Now you can run `npm run dev` in one terminal, and it will compile reason files,
and run tests, as files are written on disk.

## Syntax

Instead of using mutating nested function calls, _Respect_ uses immutable data
structures for building up the test context and tests.

```
register(
  describe "Parent context" [
    it "has some test" (fun _ =>
      ...
    )
    it "has some test" (fun _ =>
      ...
    )

    describe "Child context" [
      it "has more tests" (fun _ =>
        ...
      )
    ]
  ])
```

The only mutating construct here is the function `register` which adds the test
context to a root context.

## Async tests

The framework supports async tests through callbacks, but be aware of:

 * There is no timeout, so if you don't get to call the callback, the test suite
     will hang (luckily _nodemon_ happily kills the process on file changes).
 * The callback triggers running the rest of the test suite, so don't call it
     multiple times, or you will cause the rest of the suite to run multiple
     times, probably with simultaneously running tests.

Async support is currently best implemented by opening `Respect.Dsl.Async`.

```
open Respect.Dsl.Async;

describe "Parent context" [
  it "has an async test" (fun _ don => {
    if (success) {
      don ();
    }else {
      don err::"Error" ();
    }
  })
] |> register;
```

There is currently async matcher support through the function `shoulda`
(should-async). The function has the signature:

```
(matcher : matcher 'a 'b) => (actual : 'a) => (cb : doneCallback) => unit
```

This signature plays nicely with the callback allowing you to write tests like
this:

```
describe "Register User" [
  describe "Posting valid user" [
    it "creates a user" (fun _ => {
      createValidInput ()
        |> UserFeature.registerUser
        |> shoulda asyncSucceed
    })
  ]
] |> register
```

This is a bit cryptic but I'll try to explain

* Our test function didn't explicitly specify a done callback
* We didn't pass a done callback to to the `shoulda` function either. This makes
    the result of the `shoulda` function another function, which takes a done
    callback.
* So the result of our test function is the function returned by `should`, the
    one that takes done callback. Thus our test function has the exact shape that `it` expects.
* The `registerUser` is an async function that expects a callback that we didn't supply.
* The asyncSucceed takes an async function as argument and supplies the right
    callback that binds it to the done callback.

This doesn't play nice however, if you want to have multiple assertions in the
same test :(

It will come.

Please be aware that the matcher syntax is likely to change, but will I will try
to keep backward compatibility by moving alternate matcher framework in separate
modules.
