# ReSpect Version History

## 0.2.0

 * Breaking change: A match result can now only be asynchronous, but helper
     functions exists for matchers that evaluate synchronously. See the readme
     for guidance.

## 0.1.0

 * Support for pending tests

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
