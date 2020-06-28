---
layout: post
title:  "Make syntax highlighter for your own language"
date:   2020-06-28 01:08:00 +0200
categories: logic
permalink: /syntax_highlighter.html
---
**Contents**
* TOC
{:toc}

This will be a short bonus post about dveloping your own IDE's, sytnax highlighters and code editors. I've covered previously many posts about making compiler's for your own languages, hence it would be nice if I also showed you what to do next, onec your language is ready.

### What is it and how it works

We are now trying to develop a syntax highlighter (and all the rest code-editor development should follow relatively easily one you get this done). In general this task is nothing more than creation of a very specific context free grammar. Normally we would use flex&bison for that purpose (or ANTLR). What makes the task different this time (and unsuitable for bison/ANTLR) is that we no longer want to use code to analyse the parse tree. Instead we should use context free grammars as a kind of meta-information-tagger. For instance, we will take this code:

{% highlight C %}
int add(int x){
	int y = 7;
	return x + y;
}
{% endhighlight %}

and we will assign meta-information describing each syntactic structure, like:

{% highlight C %}
int <- type
add <- function
(   <- punctuation
int <- type
x   <- parameter
)   <- punctuation
{   <- punctuation 
int <- type
y   <- variable
=   <- separator
7   <- constant.numeric
;   <- separator.terminal
return  <- keyword.control
x   <- variable 
+   <- keyword.operator
y   <- variable
;   <- separator.terminal
}   <- punctuation
{% endhighlight %}

Later any popular code-editor framework will use this metainformation to navigate the code and implement many other useful features like syntax highlighting, context-aware auto-suggestions, syntax error detection, etc.

### Popular frameworks

You have several options to choose from. You can either take any of the extensible editors such as 

- TextMate 
- SublimeText
- VisualStudioCode
- Atom
- Vim/Emacs (if you like to do it in old-school way)

Or you can go even further and develop your own code editor with one of those web-based frameworks:

- [Ace](https://ace.c9.io/)
- [CodeMirror](https://codemirror.net)

If you want to develop native application, there might also be alternative frameworks for that, but from what I've seen, the web-based ones are easiest to work with and have best tool support. 

### Making work easier with Iro

What I recommend everyone to always use for quick prototyping of your syntax rules, is to use [Iro](https://eeyo.io/iro/). You can write your rules in their format once, and then Iro can generate a specific code for your target platform.

There have already been made extensive tutorials on this topic, so I will only give you some useful links.

- [Iro documentation and How-To](https://eeyo.io/iro/documentation/)
- [list of SublimeText meta-information tags](https://www.sublimetext.com/docs/3/syntax.html)
- [list of TextMate meta-information tags](https://macromates.com/manual/en/language_grammars#rule_keys)
- [Sample Ace editor](https://ace.c9.io/build/kitchen-sink.html) 
- [Ace online playground for syntax definition](https://ace.c9.io/tool/mode_creator.html) 

### General format in a nutshell

Every framework has slightly different format for syntax definition, however all of them are based around a few general ideas. There is a list of meta-information tags (called `styles` in Iro) and there is the grammar (specified under `contexts` below). 

{% highlight none%}
styles [] {
   .comments : style color = "light_green";
   .tree     : style color = "orange";
}
contexts[] {
   main : context {
      : pattern {
         regex    \= (\boak\b)
         styles [] = .tree;
      }
      : pattern {
         regex    \= (//.*)
         styles [] = .comments;
      }
   }
}
{% endhighlight %}

The grammar looks very different from what you would expect after working with bison/ANTLR. It isn't based on the recursive notation like:

    X ::= A | a
    A ::= b | A
    
Instead the format used by syntax highlighters is much closer to defining pushdown automata by hand. You can use regexes for convenience (which define finite state automata) but you also get to control the stack of pushdown automaton explicitly (remember that every finite state automaton is also a special case of pushdown automaton). Hence, one might say that Iro uses "regexes with stack". There are three fundamental operations (and one extra operation for convenience):

- `pattern` which advances the automaton by trying to match some regex as far as it can.
- `push` which does the job of `pattern` but also pushes new `context` onto the stack. Different regexes will be defined to apply in each context.
- `pop` which also works like `pattern` but the pops the current context from the stack
- additionally Iro has one convenience operation called `inline_push`, which allows you to specify something like "anonymous context", living only in that one rule. 

You could in theory express any grammar in this way and you _could_ convert your Bison/flex grammar to Iro, however, it doesn't mean you _should_ do it. The purpose of syntax highlighter is completely different from that of parsers found in compilers.  The `Iro` syntax is meant to be only a loosy approximation of the actualy grammar. This way, even if at some point, there is a mistake in Bison/ANTLR grammar, the Iro grammar should still be able to match the whole code and somewhat recover from the minor mistakes. Just imagine how annoying it would be if your favourite IDE worked only when the code is correct.

Here you can see an [example of editor](/code/mealy_compiler_web_and_no_leaks/web_mealy_compiler_ace.html) made by me for the (extended version of) [Mealy machine compiler](/mealy_compiler.html). The steps that I followed were:

- go to ace [GitHub page](https://github.com/ajaxorg/ace) and clone the repo `git clone https://github.com/ajaxorg/ace.git`. I will refer to the cloned directory as `$ACE_ROOT`.
- go to [Iro](https://eeyo.io/iro/) and design my syntax. Next generate Ace source code and test it [here](https://ace.c9.io/tool/mode_creator.html). If everything seems to work then proceed to next step.
- copy the code generateed by Iro and put it in `$ACE_ROOT/lib/ace/mode/$NAME_highlight_rules.js` (in my case i chose `mealy` as `$NAME` of my syntax)
- create `$ACE_ROOT/lib/ace/mode/$NAME.js` with some basic setup. You can copy one of the existing modes and make some minor changes, or you can use this template:
{% highlight Javascript%}

define(function(require, exports, module) {
"use strict";

var oop = require("../lib/oop");
var TextMode = require("./text").Mode;
// Rename MealyHighlightRules to something that matches name of your syntax
var MealyHighlightRules = require("./mealy_highlight_rules").MealyHighlightRules; // use your $NAME_highlight_rules.js instead of "mealy_highlight_rules"
var FoldMode = require("./folding/cstyle").FoldMode;

var Mode = function() {
    this.HighlightRules = MealyHighlightRules;
    this.foldingRules = new FoldMode();
    this.$behaviour = this.$defaultBehaviour;
};
oop.inherits(Mode, TextMode);

(function() {
    this.lineCommentStart = "//";
    this.$id = "ace/mode/mealy"; //rename this to match your language
    this.snippetFileId = "ace/snippets/mealy"; //rename this to match your language
}).call(Mode.prototype);

exports.Mode = Mode;
});

{% endhighlight %}
-  create `$ACE_ROOT/lib/ace/snippets/$NAME.snippets` with some basic setup 
{% highlight Javascript%}
snippet >
description function
scope mealy
	-> ${1}() = ${2};

snippet >
description if
scope mealy
	-> struct ${1} \{ ${2:**} \n \}
{% endhighlight %}
Remeber to change `scope mealy` to your `scope $NAME`.
-  create `$ACE_ROOT/lib/ace/snippets/$NAME.js` with some basic setup 
{% highlight Javascript%}
define(function(require, exports, module) {
"use strict";
exports.snippetText = require("../requirejs/text!./mealy.snippets");
exports.scope = "mealy";
});
{% endhighlight %}
Agaun, remember to change `mealy.snippets` to `$NAME.snippets` and `exports.scope = "mealy"` to `exports.scope = "$NAME"`
-  add your language to `$ACE_ROOT/lib/ace/ext/modelist.js`. In my case I inserted a new line like `Mealy:       ["mealy"],`.
- (optional) you may now test your Ace editor by running `node static.js --allow-save` in `$ACE_ROOT` and going to `http://localhost:8888/tool/mode_creator.html` and `http://localhost:8888/build/kitchen-sink.html`.
-  run `npm install` in `$ACE_ROOT`
-  run `node ./Makefile.dryice.js full` in `$ACE_ROOT`. This should create `$ACE_ROOT/build/src`. You should even see there your syntax `$ACE_ROOT/build/src/mode-$NAME.js`.
- Copy `$ACE_ROOT/build/src` to your project directory. In my case I added it to `code/ace/build/src`, which you can even see on [GitHub](https://github.com/aleksander-mendoza/aleksander-mendoza.github.io/tree/master/code/ace/build/src).
- Finally embed ace in your HTML file like in [this example](https://ace.c9.io/#nav=embedding). Notice that  `$ACE_ROOT/build/src` contains `ace.js`.
{% highlight html %}
<!doctype html>
<html lang="en-us">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <title>SyntaxHighlighter</title>
    <style type="text/css" media="screen">
    #editor { 
        position: absolute;
        top: 0;
        right: 0;
        bottom: 0;
        left: 0;
    }
    </style>
  </head>
  <body>
    <div id="editor">A = [da];</div>
    <script type="text/javascript" src="../ace/build/src/ace.js"></script>
    <script>
        var editor = ace.edit("editor");
        editor.setTheme("ace/theme/monokai");
        
        editor.session.setMode("ace/mode/mealy");
    </script>
  </body>
</html>
{% endhighlight %}

