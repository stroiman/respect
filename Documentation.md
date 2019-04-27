# Respect documentation

## Installation

This guide will help you get `Respect` and having a test-watcher setup.
_Respect_ in itself does not implement test-watcher functionality, but it is
easily added with the _nodemon_ package.

### Basic installation

First, add _respect_, the npm package is named "@stroiman/respect"

```shell
npm install --save-dev @stroiman/respect
```

As this is a package with Reason code, you need to add a reference to the
package in the _bsconfig.json_ file, as well.

You also need a folder to contain your test files.

```json
"files": [
  {"dir": "src"},
  {"dir": "tests",
   "type": "dev" }
],
"bs-dev-dependencies": [
  "@stroiman/respect"
]
```

Create a skeleton test, "./tests/tests.re":

```Reason
open Respect.Dsl;

describe "My first test" [
  it "runs" (fun _ => {()})
] |> register
```

The functions `describe` and `it` helps build an immutable data structure
describing your tests. `register` adds this to a global list, so they can be
found by the runner.

Now, let's add a test target to _package.json_ to call the test runner. The
runner needs to find the compiled `.js` files. By default, it looks in
`lib/js/tests/**/*.js` - here I have made it explicit:

```Reason
"scripts": {
   ...
   "test": "respect lib/js/tests/**/*.js"
}
```

Execute `npm run build` to build the code, and `npm run test` to run the tests.

### Adding test watcher functionality

The npm package _nodemon_ can trigger running _.js_ files when the file system
changes. We can use this to implement filesystem watcher functionality. First
install the package

```Reason
npm install --save-dev nodemon
```

And then add a script to the _package.json_ file (remember to add a file-glob if
the tests are not in `lib/js/tests`.

```Reason
  "scripts": {
    ...
    "test:watch": "nodemon node_modules/.bin/respect"
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

```Reason
npm install --save-dev npm-run-all
```

In the _package.json_ file, add a new script:

```Reason
  "scripts": {
    ...
    "dev": "run-p watch test:watch"
  }
```

The command `run-p` is part of _npm-run-all_, and it runs the two scripts in
parallel.

Now you can run `npm run dev` in one terminal, and it will compile reason files,
and run tests, as files are written on disk.

**Caution** When implementing this target, you can experience false positives.
The tests are executed whenever a `.js` file has changed. And sometimes when the
bucklescript build fails, it still touches some of the `.js` files, causing a
test run to execute, when we don't want it to.

## Syntax

Instead of using mutating nested function calls, _Respect_ uses immutable data
structures for building up the test context and tests. Therefore, the
`desribe`-operation takes nested operations in a list.

```Reason
register(
  describe("Parent context", [
    it("has some test", (_) =>
      ...
    ),

    it("has some test", (_) =>
      ...
    ),

    describe("Child context", [
      it("has more tests", (_) =>
        ...
      )
    ])
  ])
```

The only mutating construct here is the function `register` which adds the group
of examples to an implicit root group.

### Pending tests

Often it is useful to write pending tests, small skeleton desrciptions of
functionality you need to implement. This can turn the test framework into a
small todo list:

```Reason
describe("Register user", [
  pending("Returns Ok(user) if registration succeeded"),
  pending("Returns Error(DuplicateEmail) if email already registered"),
]) |> register
```

Pending tests will not result in failure when running the tests.

## Async tests

Async support is currently best implemented by opening `Respect.Dsl.Async`.

```Reason
open Respect.Dsl.Async;

describe ("Parent context", [
  it("has an async test", (_,don) => {
    if (success) {
      don ()
    }else {
      don (~err="Error",())
    }
  })
]) |> register;
```

There is currently async matcher support through the function `shoulda`
(should-async). The function has the signature:

```Reason
(matcher : matcher 'a 'b) => (actual : 'a) => (cb : doneCallback) => unit
```

This signature plays nicely with the callback allowing you to write tests like
this:

```Reason
describe("Register User", [
  describe("Posting valid user", [
    it("creates a user", (_) => {
      createValidInput ()
        |> UserFeature.registerUser
        |> shoulda(asyncSucceed)
    })
  ])
]) |> register
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

Please be aware that the matcher syntax is likely to change, but I will try
to keep backward compatibility by moving alternate matcher framework into separate
modules.

## Focused/skipped examples

You can focus an example with the `focus` function. When there are focused
examples, only those will be executed.

You can skip an example with the `skip` function. Skipped examples will not run.

You can apply the functions to both examples, and example groups.

```reason
describe("Group with focused examples", [
  focus @@
  it("This example is focused", ...),

  skip @@
  it("This example is skipped", ...),

  focus @@
  describe("Group with more focused examples", [
    it("This example is focused", ...),

    it("This example is also focused", ...),
  ])
]) |> register
```

## Matchers

The matchers framework is based on these types:

```Reason
type matchResult('t) =
  | MatchSuccess('t)
  | MatchFailure(Obj.t);

type matcher('a, 'b) = 'a => (matchResult('b) => unit) => unit;

exception MatchFailedException(string);
```

So a matcher takes an actual value and provides a matchresult asyncrounously
through a callback. Matchers that evaluate synchronously can use these helper
functions

```Reason
let matchSuccess = (a) => cb => cb(MatchSuccess(a));
let matchFailure = (a) => cb => cb(MatchFailure(a |> Obj.repr));
```

So if we look at the `equal` match constructor:

```Reason
let equal = (expected, actual) =>
  actual == expected ? matchSuccess(actual) : matchFailure(expected);
```

So it takes an expected value and returns a matcher based on this.

### Composing Matchers

Matchers can be composed using the "fish" operator `>=>`, so a `matcher('a,'b)`
can be composed with a `matcher('b,'c)` into a `matcher('a,'c)`.

This can be particularly useful when the value passed with the success is
different from the actual value passed to the matcher. Here is an example from a
piece of production code I am working on:

```Reason
/* General types to handle errors and async code */
type result('a, 'b) = Js.Result.t('a, 'b) = | Ok('a) | Error('b);

type async('a) = ('a => unit) => unit;
type asyncResult('a,'b) = async(result('a,'b));

/* Specific error types returned by repository layer */
type databaseError 'id =
  | DocumentNotFound(string,'id)
  | MongoErr(MongoError.t);

/* This is a matcher that verifies that an async function fails. "actual" is a
function that takes a result callback */
let asyncFail = actual => cb => {
  actual
    |> AsyncResult.run (fun
    | Error(y) => cb(MatchSuccess(y))
    | Ok(y) => cb(MatchFailure (Obj.repr(y))));
};
```

The interesting thing is that the `asyncFail` matcher passes the error to the
`MatchResult` constructor, to be used by a new matcher. In this tests we compose
it with a new matcher that verifies that we actually get the expected error.

```Reason
describe("UserRepository", [
  describe("findById", [
    describe("record doesn't exist", [
      it("returns DocumentNotFound", (_) => {
        let id = "dummy";
        UserRepository.getById(id)
          |> shoulda (asyncFail >=> (equal (DocumentNotFound("users",id))))
      })
    ])
  ])
]) |> register;
```

## Test Metadata

You can add metadata to a group or an example. And if you have metadata on a
parent group, you can override it in a child group. The metadata is added using
the strange looking `**>` operator (I chose this because the `**` makes it right
associative, which I need in order to avoid parenthesis hell, and the `>` is a
visual aid, that it binds with the group to come.
indicating that the metadata binds to the group/example to come.

The interesting thing is that the metadata is initialized before the example
starts executing, which means that metadata specified on an example can effect
the setup code executed in a parent group. The following example shows how:

```Reason
open Respect.Dsl.Async;
module Ctx = Respect.Ctx;

describe("Register user", [
  beforeEach ((ctx,don) => {
    ctx
      |> Ctx.get("userName")
      |> /* do something interesting with the user */
    don()
  }),

  ("userName", "johndoe") **>
  describe("A valid user name was entered", [
    it("Correctly registers the user", (ctx,don) => {
       ...
       don
    })
  ]),

  ("userName", "!@#$") **>
  describe("An invalid user name was entered", [
    it("Returns a sensible error message", (ctx, don) => {
       ...
       don ()
    })
  ])
]) |> register
```

Multiple pieces of metadata can be added to the same example or group, and
values can be overwritten in nested groups/examples.

```Reason
/* Pass sensible defaults for a happy case to the root example */
("userName", "johndoe") **>
("password", "agoodlongpassword*42!X") **>
describe("Register user", [
  beforeEach((ctx, don) => {
    let userName = ctx |> Ctx.get("userName");
    let password = ctx |> Ctx.get("password");
  }),

  it("succeeds when username and password are ok", (ctx, don) => {
  }),

  /* Create various examples that deviate from the happy case to test
     that the code handles these cases correctly */

  ("userName", "!@#$") **>
  it("Rejects the attempt when username is invalid", (ctx, don) => {
    ...
  }),

  ("password", "xyz") **>
  it("Rejects the attempt when password is too short", (ctx, don) => {
  })
]) |> register
```

## TestContext

The context object passed to both setup functions and test functions provides a
place setup code can place data that can be read by examples or setup code in
nested contexts.

Currently, the data in the test context is mutable, but that may change.

Currently, all data stored in the test context is stored as `Obj.t`.
Unfortunately, there is no run-time checking that when you retrieve data from
the context, that the types match. For example the following code will neither
generate a compile, nor a run-time error.

```Reason
let newCtx = ctx |> Ctx.add("key", 42);
let s : string = newCtx |> Ctx.get("key");
```

We just assigned a number to a string value. This will probably generate a hard
to diagnose exception further down the line. And a particular annoying exception
because the bug is in the test code, not the production code.

I would appriciate all ideas to come with a better suggesion for a better type
safe data store. Perhaps polymorphic variants?

#### Subject

A special property exists on the test context, the `subject` which is meant to
mean "the thing that we are verifying". The subject is a lazily evaluated value,
and the evaluation receives the test context as input. Therefore a setup
function in a parent group can setup the subject, and setup functions in nested
groups can modify the input.

This shows how (this is for the sake of the example only, there are nicer ways
of doing this).
```Reason
describe("create user", [
  beforeEach((ctx) => {
    ctx
      |> Ctx.setSubj(ctx => {
          let username : string = ctx |> Ctx.get("username");
          let password : string = ctx |> Ctx.get("password");
          createUser({username, password});
          /* Assume that createuser returns an Ok/Error indicating the result */
      })
      |> Ctx.don
  }),

  describe("Valid credentials", [
    /* this nested context modifies the context before the subject evaluation */
    beforeEach(ctx => {
      ctx
        |> Ctx.add("username", "validusername")
        |> Ctx.add("password", "validpassword")
    })

    it("successfully creates the user", ctx => {
      ctx
        |> Ctx.subject
        |> shoulda(equal(Ok({...})))
    })
  ])
])
```

#### Ctx module functions

```Reason

/* The don function helps return a curried function for the done callback */
beforeEach(ctx => ctx |> Ctx.don)

/* Adding data to the context */
beforeEach(ctx => ctx
  |> Ctx.add("Key", value)
  |> Ctx.add("key", value)
  |> Ctx.don);

/* Retrieving data */
let x : string = ctx |> Ctx.get("key")

/* Retrieve Some(_) if a key exists, None if it doesn't */
let x : option(string) = ctx |> Ctx.tryGet("key")

/* Mapping data in the context */
describe("email is empty", [
  beforeEach(ctx => ctx
    |> Ctx.map("user", user => {...user, email: ""})
    |> Ctx.don)
])

/* Setting the subject */
beforeEach(ctx => ctx
  |> Ctx.setSubj(ctx => {
    Login(ctx |> Ctx.get("username"), ctx |> Ctx.get("password"))
  })
  |> Ctx.don)

/* Getting the subject */
let subject : option(Domain.user) = ctx |> Ctx.subject;
```

#### Test context as an object

The test context itself is implemented as an object, meaning that you can use
the object methods to access the data.

This could be changed however, so I would recommend using the module function.

```Reason
ctx#add("key", value)
/* add returns the context, making it possible to chain calls to #add */
ctx#add("key", value)#add("key2", value2) /* do notice the bug mentioned below */

let s: string = ctx#get("key")
/* get will automatically cast to whatever type you assign it to */

let maybeS: option(string) = ctx#tryGet("key")
/* returns Some(_) if the key exists, otherwise returns None */

ctx#setSub(ctx => Login(ctx#get("username"), ctx#get("password")))
/* Sets the function that will be used to evaluate the subject. The function
 receives the context (which may have been updated) when it is eventually run */

let s = ctx#subject()
/* Evaluates and retrieves the subject. The value is cached, so multiple calls
will return the same value. */

ctx#don()
/* helper for ending a setup function */

beforeEach(ctx => {
  ctx
    #add("key", 42)
    #don()
})
```

### Extending the context

One technique to help reduce code duplication in the test is to write a
specialed context module in a code file with tests, and add useful functions to
this.

```Reason
open Respect.Dsl.Async;
open Respect.Matcher;

module Ctx = {
  include Respect.Ctx;

  let createUser = ctx => {
    open CreateUserModule;
    let username = ctx |> get("username");
    let password = ctx |> get("password");
    createUser({username, password})
  }
};

("username", "goodusername") **>
("password", "goodpassword") **>
describe("createUser", [
  it("succeeds when user is a success", ctx =>
    ctx
      |> Ctx.createUser
      |> shoulda(beSuccess))

  ("username", "") **>
  it("fails when username is empty", ctx =>
    ctx
      |> Ctx.createUser
      |> shoulda(beFailure))
])
```
