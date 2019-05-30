# ReSpect

BDD-style test framework for Reason/bucklescript

__Attention__ - _The NPM package has moved to `@stroiman/respect`. Remember to
update both _package.json_ AND `bsconfig.json`._

_The source repo was moved from to `https://github.com/stroiman/respect.git`_

[![Build Status](https://travis-ci.org/stroiman/respect.svg?branch=master)](https://travis-ci.org/stroiman/respect)

This is an RSpec inspired test framework for ReasonML/OCaml/Bucklescript. The
runner uses raw javascript code, so it will only run in node environments at the moment.

I base this on a lot of experience I gained from a similar project for F#, FSpec.

This project is still in a very early stage, so use at your own risk. Breaking
changes occur from time to time.

## Features

 * The framework places tests in "examples", which are grouped in "example
     groups", just like mocha, jest, jasmine, etc.
 * Example groups and their examples are build using immutable data structures,
     allowing for metaprogramming to modify/generate the examples.
 * Support for testing async code.
 * The framework provides a context object that is unique to each individual
     test case, and provides a place where each test case can store state
     necessary for that case.
 * Each example or group can have metadata atteched that will be made available
     through the context object. This can be used modify what happens in the
     setup code.

## Getting Started

Run `npm install --save-dev @stroiman/respect` and add `@stroiman/respect` to the `bs-dev-dependencies` in `bsconfig.json`.

```Reason
open Respect.Dsl.Sync;

describe "My first test" [
  it "should be a failing test" (fun _ => {
    1 |> should (equal (2))
  })
] |> register
```

Add a test script to `package.json`:

```json
"scripts": {
   ...
   "test": "respect"
}
```

Build the code and run the tests with: `npm run test`

For more info, including tips on test watcher functionality, see the full 
[Documentation](https://github.com/PeteProgrammer/respect/blob/master/Documentation.md)

## TODO

 * "Finalize" DSL for building test suites.
 * Determine whether or not to allow mutation of `TestContext`. This worked well
     in F#/FSpec, but we don't have runtime type checking in Reason/Bucklescript.
 * Finalize assertion framework.
 * Nicer test output when running.
 * Nicer test output when assertions fail.
 * Internally, figure out how to report progresss.
 * More flexible runner, e.g. configurable location of test files
 * ✓ Handle async timeout to avoid hanging when async tests don't call back
 * Make timeout configurable through example metadata.
 * Support tear-down code

Although, I had learned from many mistakes when building FSpec, there are some
problems that demand different solutions in Reason/Bucklescript. Async support
in particular.

