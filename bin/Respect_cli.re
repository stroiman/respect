[%%bs.raw
  {|
const glob = require('glob');
const path = require('path');

const files = glob.sync("lib/js/tests/*.js");

for (let f of files) {
  require(path.resolve(f));
} |}
];

Respect.Runner.runRoot ();
