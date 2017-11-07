// Generated by BUCKLESCRIPT VERSION 2.0.0, PLEASE EDIT WITH CARE
'use strict';

var $$Map    = require("bs-platform/lib/js/map.js");
var Curry    = require("bs-platform/lib/js/curry.js");
var $$String = require("bs-platform/lib/js/string.js");

var include = $$Map.Make([$$String.compare]);

var empty = include[0];

var add = include[3];

var merge = include[6];

var find = include[21];

function merge$1(a, b) {
  return Curry._3(merge, (function (_, x, y) {
                if (x) {
                  return x;
                } else {
                  return y;
                }
              }), a, b);
}

var ContextMap_001 = /* is_empty */include[1];

var ContextMap_002 = /* mem */include[2];

var ContextMap_004 = /* singleton */include[4];

var ContextMap_005 = /* remove */include[5];

var ContextMap_006 = /* compare */include[7];

var ContextMap_007 = /* equal */include[8];

var ContextMap_008 = /* iter */include[9];

var ContextMap_009 = /* fold */include[10];

var ContextMap_010 = /* for_all */include[11];

var ContextMap_011 = /* exists */include[12];

var ContextMap_012 = /* filter */include[13];

var ContextMap_013 = /* partition */include[14];

var ContextMap_014 = /* cardinal */include[15];

var ContextMap_015 = /* bindings */include[16];

var ContextMap_016 = /* min_binding */include[17];

var ContextMap_017 = /* max_binding */include[18];

var ContextMap_018 = /* choose */include[19];

var ContextMap_019 = /* split */include[20];

var ContextMap_021 = /* map */include[22];

var ContextMap_022 = /* mapi */include[23];

var ContextMap = /* module */[
  /* empty */empty,
  ContextMap_001,
  ContextMap_002,
  /* add */add,
  ContextMap_004,
  ContextMap_005,
  ContextMap_006,
  ContextMap_007,
  ContextMap_008,
  ContextMap_009,
  ContextMap_010,
  ContextMap_011,
  ContextMap_012,
  ContextMap_013,
  ContextMap_014,
  ContextMap_015,
  ContextMap_016,
  ContextMap_017,
  ContextMap_018,
  ContextMap_019,
  /* find */find,
  ContextMap_021,
  ContextMap_022,
  /* merge */merge$1
];

function add$1(key, x, t) {
  t[/* data */0] = Curry._3(add, key, x, t[/* data */0]);
  return /* () */0;
}

function get(key, t) {
  return Curry._2(find, key, t[/* data */0]);
}

function create() {
  return /* record */[/* data */empty];
}

exports.ContextMap = ContextMap;
exports.add        = add$1;
exports.get        = get;
exports.create     = create;
/* include Not a pure module */