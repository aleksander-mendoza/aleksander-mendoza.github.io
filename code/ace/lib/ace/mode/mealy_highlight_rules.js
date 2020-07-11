/*
* To try in Ace editor, copy and paste into the mode creator
* here : http://ace.c9.io/tool/mode_creator.html
*/

define(function(require, exports, module) {
"use strict";
var oop = require("../lib/oop");
var TextHighlightRules = require("./text_highlight_rules").TextHighlightRules;
/* --------------------- START ----------------------------- */
var MealyHighlightRules = function() {
this.$rules = {
"start" : [
   {
      "token" : ["entity.name.type", "none", "meta.separator", "none", "punctuation"],
      "regex" : "([a-zA-Z0-9_]+)(\\s*)(=)(\\s*)(\\[)",
      "push" : "alphabet"
   },
   {
      "token" : "comment",
      "regex" : "(//[^\\n]*)"
   },
   {
      "token" : "meta.separator",
      "regex" : "(;)"
   },
   {
      "token" : ["variable", "none", "punctuation", "none", "punctuation", "none", "meta.separator"],
      "regex" : "([a-zA-Z0-9_]+)(\\s*)(\\()(\\s*)(\\))(\\s*)(=)",
      "push" : "function"
   },
   {
      "token" : ["entity.name.type", "none", "meta.separator"],
      "regex" : "([a-zA-Z0-9_]+)(\\s*)(:)",
      "push" : "type"
   },
   {
      "token" : ["keyword", "none", "entity.name.type", "none", "punctuation"],
      "regex" : "(struct)(\\s+)([a-zA-Z0-9_]+)(\\s*)(\\{)",
      "push" : "struct"
   },
   {
      defaultToken : "text",
   }
], 
"alphabet" : [
   {
      "token" : ["punctuation", "none", "punctuation"],
      "regex" : "(\\])(\\s*)(;)",
      "next" : "pop"
   },
   {
      "token" : "keyword",
      "regex" : "(#)"
   },
   {
      "token" : "constant.character.escape",
      "regex" : "(\\\\.)"
   },
   {
      "token" : "text",
      "regex" : "(.)"
   },
   {
      defaultToken : "text",
   }
], 
"function" : [
   {
      "token" : "meta.separator",
      "regex" : "(;)",
      "next" : "pop"
   },
   {
      "token" : "constant.numeric",
      "regex" : "(\\b[0-9]+\\b)"
   },
   {
      "token" : "keyword",
      "regex" : "(->|\\*)"
   },
   {
      "token" : "keyword",
      "regex" : "(:|\\*|\\+|\\|)"
   },
   {
      "token" : "punctuation",
      "regex" : "(\\(|\\))"
   },
   {
      "token" : "string.regexp",
      "regex" : "(\\[([^\\\\\\[\\]\\-]|\\\\.)\\-([^\\\\\\[\\]\\-]|\\\\.)\\])"
   },
   {
      "token" : "variable",
      "regex" : "([a-zA-Z0-9_]+)"
   },
   {
      "token" : "punctuation",
      "regex" : "(\")",
      "push" : "string"
   },
   {
      defaultToken : "text",
   }
], 
"string" : [
   {
      "token" : "punctuation",
      "regex" : "(\")",
      "next" : "pop"
   },
   {
      "token" : "keyword",
      "regex" : "(#)"
   },
   {
      "token" : "constant.character.escape",
      "regex" : "(\\\\.)"
   },
   {
      "token" : "string",
      "regex" : "(.)"
   },
   {
      defaultToken : "text",
   }
], 
"struct" : [
   {
      "token" : "punctuation",
      "regex" : "(})",
      "next" : "pop"
   },
   {
      "token" : ["variable", "none", "meta.separator", "none", "entity.name.type", "none", "keyword"],
      "regex" : "([a-zA-Z0-9_]+)(\\s*)(:)(\\s*)([a-zA-Z0-9_]+)(\\s*)(\\*?)"
   },
   {
      "token" : "meta.separator",
      "regex" : "(,)"
   },
   {
      defaultToken : "text",
   }
], 
"type" : [
   {
      "token" : "meta.separator",
      "regex" : "(;)",
      "next" : "pop"
   },
   {
      "token" : "entity.name.type",
      "regex" : "([a-zA-Z0-9_]+)"
   },
   {
      "token" : "keyword",
      "regex" : "(->|\\*)"
   },
   {
      defaultToken : "text",
   }
]
};
this.normalizeRules();
};
/* ------------------------ END ------------------------------ */
oop.inherits(MealyHighlightRules, TextHighlightRules);
exports.MealyHighlightRules = MealyHighlightRules;
});
