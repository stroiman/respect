# ReSpect

[![Build Status](https://travis-ci.org/PeteProgrammer/respect.svg?branch=master)](https://travis-ci.org/PeteProgrammer/respect)

This is an RSpec inspired test framework for ReasonML/OCaml/Bucklescript. The
runner uses raw javascript code, so it will only run in node environments at the moment.

I base this on a lot of experience I gained from a similar project for F#, FSpec.

This project is still in a very early stage, so use at your own risk.

More [Documentation](https://github.com/PeteProgrammer/respect/blob/master/Documentation.md)

## Latest changes

### 0.5.0

 * Breaking change: Renamed `TestContext` module to `Respect_ctx` and made it
     available under `Respect.Ctx`. A quick fix to get your code to compile
     is to add the this to your own modules `module TestContext = Respect.Ctx`.

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

Although, I had learned from many mistakes when building FSpec, there are some
problems that demand different solutions in Reason/Bucklescript. Async support
in particular.


