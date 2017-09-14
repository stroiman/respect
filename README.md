# ReSect

This is an RSpec inspired test framework for ReadonML/OCaml.

I base this on a lot of experience I gained from a similar project for F#, FSpec

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

