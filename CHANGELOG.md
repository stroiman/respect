# ReSpect Version History

### 0.9.0

 * Add support for focused and skipped examples
 * Upgrade to bs-platform@5

### 0.8.0

 * Remove compiler warnings caused by module aliases
 * Support being used in projects compiled with inSource: true
 * Fix source file glob

### 0.7.2

 * Update `@stroiman/async` to 0.7

### 0.7.1

 * Attempt to fix missing documentation, links, etc. on npmjs.com

### 0.7.0

 * Changed dependency from `re-sync` to `@stroiman/async`
 * Build against bs-platform 2.2.1

### 0.6.0

 * cli now allows you to specify test files on the command line. (was hardcoded
     to `lib/js/tests/*.js` before.

### 0.5.4

 * Moved package from `re-respect` to `@stroiman/respect`.

### 0.5.1

 * No of passed, pending, and failed tests are written to the console after the
     test run.

### 0.5.0

 * Breaking change: Renamed `TestContext` module to `Respect_ctx` and made it
     available under `Respect.Ctx`. A quick fix to get your code to compile
     is to add the this to your own modules `module TestContext = Respect.Ctx`.

### 0.4.3

 * Added `Ctx.tryGet` - Returns `None` if no entry with the specified key exists
     in the context data.

### 0.4.2

 * Added Respect.Ctx module as alias for TestContext

### 0.4.1

 * TestContext.don function to help accept a done callback from setup functions

### 0.4.0

 * Refactor: Test context is now represented by an object instead of a mutable
     record. But the TestContext module still has functions for operating on the
     actual context, so existing code should be compatible.
 * Testcontext subject: The subject is a function that evaluates to an actual
     value when requested. The function receives the test context as input. You
     can assign the subject in a parent group, and modify the context in a child
     group.
 * TextContext.map: Allows you to easily modify objects in the context.

### 0.3.0

 * Breaking change: In order to get better error messages when match fails, the
     MatchFailure constructor now takes two args, the actual object, and the
     expected object of the failed matcher. This was in particular for testing
     async results.

## 0.2.0

 * Breaking change: A match result can now only be asynchronous, but helper
     functions exists for matchers that evaluate synchronously. See the readme
     for guidance.

## 0.1.0

 * Support for pending tests

## 0.0.9

 * Removed dependency to 're-sync' npm package. This caused cyclic deps
     when building 're-sync'

## 0.0.8

 * Only show failed tests in the console
 * Added a 1 second timout for examples (not yet configurable)
 * Bugfix: Test runniner wasn't working on case sensitive file system

## 0.0.6

 * Added test metadata support

## 0.0.5

 * Implemented nicer callback for tests `don ()` or `don err::"Failed" ()`
 * Created `Respect.Dsl.Async` module to have nice naming for async test suites.
 * Implemented async support in assertion framework.

## 0.0.4

 * Rewrote runner engine to use continuations intenally to allow for async tests.
 * Created an `it_a` construct to create async tests.

## 0.0.3

First version with a test runner that was able to execute tests
