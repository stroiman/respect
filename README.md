# ReSect

This is an RSpec inspired test framework for ReadonML/OCaml.

I base this on a lot of experience I gained from a similar project for F#, FSpec

## Installation

This guide will help you get `Respect` and having a test-watcher setup. In
addition to `Repect`, you need these two packages:
 * `nodemon` - This package implements file system watcher functionality.
 * `npm-run-all` - (optional) Allows us to easily run build and tests from one npm command.

```
npm install --save-dev respect
npm install --save-dev nodemon
npm install --save-dev npm-run-all
```

You need to register `respect` in you bsconfig file

```
"files": [
  {"dir": "src"},
  {"dir": "tests",
   "type": "dev" }
],
"bs-dependencies": [
  "respect"
]
```

Create a skeleton test, "./tests/tests.re":

```
open Dsl;

describe "My first test" [
  it "runs" (fun _ => {()})
] |> register

!rootContext |> run
```

Add npm script, for example:

```
  "scripts": {
    "clean": "bsb -clean-world",
    "build": "bsb -make-world",
    "watch": "bsb -make-world -w",
    "test:watch": "nodemon ./lib/js/tests/root.js",
    "dev": "run-p test:watch watch"
  }
```

And now, you can have the tests run automatically when a source file changes
with the command `npm run dev`

## Syntax

Instead of using nested function calls as both RSpec, and it JavaScript clones,
Jasmine, Mocha, Jest, etc, this uses immutable data structures to build up the
test hierarcy.

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

I'm still quite new to ReasonML, so time will tell if I can bring my experiences
from FSpec to this project

