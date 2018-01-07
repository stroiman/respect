# ReSpect

[![Build Status](https://travis-ci.org/PeteProgrammer/respect.svg?branch=master)](https://travis-ci.org/PeteProgrammer/respect)

This is an RSpec inspired test framework for ReasonML/OCaml/Bucklescript. The
runner uses raw javascript code, so it will only run in node environments at the moment.

I base this on a lot of experience I gained from a similar project for F#, FSpec.

This project is still in a very early stage, so use at your own risk.

## Latest changes

### 0.4.0 - 0.4.3

 * Added `Ctx.tryGet` - Returns `None` if no entry with the specified key exists
     in the context data.
 * Added Respect.Ctx module as alias for TestContext
 * TestContext.don function to help accept a done callback from setup functions
 * Refactor: Test context is now represented by an object instead of a mutable
     record. But the TestContext module still has functions for operating on the
     actual context, so existing code should be compatible.
 * Testcontext subject: The subject is a function that evaluates to an actual
     value when requested. The function receives the test context as input. You
     can assign the subject in a parent group, and modify the context in a child
     group.
 * TextContext.map: Allows you to easily modify objects in the context.

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

