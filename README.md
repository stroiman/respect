# ReSpect

This is an RSpec inspired test framework for ReasonML/OCaml.

I base this on a lot of experience I gained from a similar project for F#, FSpec

## Installation

This guide will help you get `Respect` and having a test-watcher setup.
_Respect_ in itself does not implement test-watcher functionality, but it is
easily added with the _nodemon_ package.

### Basic installation

First, add _respect_, the npm package is named "re-respect"

```
npm install --save-dev re-respect
```

As this is a package with Reason code, you need to add a refenrence to the
package in the _bsconfig.json_ file, as well.

You also need to add a _tests_ folder to contain the tests. At this early time
of writing, the only place that Respect searches for tests files is in the test
folder.

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

