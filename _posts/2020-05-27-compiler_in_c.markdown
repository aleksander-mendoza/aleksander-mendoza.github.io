---
layout: post
title:  "Quick compiler tutorial (C version)"
date:   2020-05-27 01:07:00 +0200
categories: compilers
permalink: /compiler_in_c.html
---

### Making compilers is easy

People often think of compiler design as sort of black magic. Some arcane craft known by very few. In reality it's far simpler than that.  It's all about languages and as we all know, languages are the essence of computer science. A compiler is nothing more than just translator for machines. You can make a compiler that converts Turing machines to lambda calculus, or string rewrite systems to recursive functions. In one of the [previous posts](/compiling_regexes.html) I already explained how to translate regular expressions to finite state machines. That was in essence a compiler. 

### Toy language

The end goal of this tutorial is creating a very simple parser for a toy language, which can evaluate arithmetic expressions like for example

    1+2/4*6+(1+3*(-4))
    
    
If you succeed at finishing this tutorial, they you will be albe to move on to a more ambitious project. 

### Setup

This tutorial will show you how to make your first compiler in C with flex&bison. For convenience I will use CMake as build system. 

    /
    |-- src/
    |   |-- main.c
    |   |-- syntax_parser.y
    |   |-- syntax_scanner.l
    |   |-- CMakeLists.txt
    |   `-- include/
    |
    |-- compile.sh
    `-- run.sh
    

In `CMakeLists.txt` put


{% highlight cmake %}
cmake_minimum_required(VERSION 3.10)

# set the project name and language
project(FirstCompiler LANGUAGES C VERSION 0.1 )
# Add Bison and Flex support
find_package(BISON)
find_package(FLEX)
# Tell bison where to find grammar files
BISON_TARGET(SyntaxParser syntax_parser.y ${CMAKE_CURRENT_BINARY_DIR}/syntax_parser.c)
# Tell Flex where to find tokenizer files
FLEX_TARGET(SyntaxScanner syntax_scanner.l  ${CMAKE_CURRENT_BINARY_DIR}/syntax_scanner.c)
ADD_FLEX_BISON_DEPENDENCY(SyntaxScanner SyntaxParser)
# add the executable
add_executable(first_compiler
    main.c
    ${BISON_SyntaxParser_OUTPUTS}
    ${FLEX_SyntaxScanner_OUTPUTS}
)
# Optionally add directories to include, but we won't actually use it here.
target_include_directories(first_compiler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
# Set C version
set_property(TARGET first_compiler PROPERTY C_STANDARD 11)
# This will help you with debugging errors
set(CMAKE_VERBOSE_MAKEFILE ON)

get_target_property(OUT first_compiler LINK_LIBRARIES)
message(STATUS ${OUT})
    

{% endhighlight %}

In order to run CMake we will use this helpful code from `compile.sh` script

{% highlight bash %}
#!/bin/bash
if [ ! -e bin ];then # make sure 'bin' directory exists
  mkdir bin # if there was no 'bin' directory then create it
  cd bin # then go to 'bin'
  cmake ../src # and initialize cmake
  cd .. # go back to root of the project
fi 
# now CMake should be set up in 'bin'
cd bin
cmake --build . # Tell CMake to build our project
{% endhighlight %}

If we (sucessfully) compile our C project, we should be able to run our compiler with this script:

{% highlight bash %}
#!/bin/bash
bin/first_compiler # run executable
{% endhighlight %}

In order to clean and refresh the project (for instance, if you think that some files are corrupted and you want to start clean) you can just delete `bin` directory and run `./compile.sh` again.

Of course at this moment it doesn't make any sense to compile anything. We still haven't written a single line of C code. Let's begin then!

### First minimal sample project with Flex and Bison


Before we are able to compile anything, we first need to parse the source code of our language


![ast1234](/assets/ast_1_plus_2_plus_3_plus_4.png){: style="float: left"}  

The first stage of parsing is lexing. It splits the strings of characters into tokens. Those token will then be used as atomic building blocks (terminal symbols) of grammars during the second stage of parsing. The final result of parsing should be a data structure known as _abstract sytanx tree_ (AST for short). It's nothing more than a special kind of tree graph in which all nodes correspond to some syntactic and logical structures. Take for example the expression `1+2+3+4`. The resulting AST should look like presented on the left. 
Notice that the because addition is commutataive (the order `x+y` or `y+x` makes no difference) the tree might have a different shape and still be logically equivalent. However this is a very special scenario. Most of the time the order will actually matter a lot. The
picture on the left represents the following bracketing: `1+(2+(3+4))`. The result of addition would still be the same even if bracketing was `(1+2)+(3+4)`, but imagine what would happen iff instead of addition it was division `1/2/3/4`. There the order of operations will make huge difference.

This post will not go into deeper details of formal grammars and theory behind parsing. If you want to know more I suggest you check out [this post](/parsers.html). Now let's get our hands dirty. Writing a lexer manually is very difficult and error-prone. Therefore we shall use Flex to generate the lexer code for us. Below are the contents of `syntax_scanner.l` file.




{% highlight flex %}

%{ 
/*This is the beginning of code section 
that flex will insert to generated lexer.
You can include here anything you need*/

#include "syntax_parser.h" //We will add this soon. 
// PLUS, MINUS, DIV, MUL, BEG, END, 
// NUM will be defined in syntax_parser.h
%}
/**Below you can define regular expressions*/
blanks          [ \t\n]+ 
number          [0-9]+

/*noyywrap is not very importnant but without it thing will break*/
%option noyywrap  

%% /**this marks beginning of lexer rules*/

{blanks}    { /* ignore whitespaces matched by "blanks" regex*/ }


"+"	    return(PLUS); //when lexer matches + symbol it will return PLUS token
"("	    return(BEG);
")"	    return(END);
"-"	    return(MINUS);
"/"	    return(DIV);
"*"	    return(MUL);

{number}	{ 
       // The NUM token will hold extra information 
       // stored in yylval.ival
       char * end = yytext+strlen(yytext); 
       // yytext holds text matched by "number" regex
       yylval.ival = strtol(yytext,  &end, 10);
       return(NUM);
}

{% endhighlight %}


Now it's time to add `syntax_parser.y` and it will become clear where
alle the `PLUS`, `MINUS`,...,`NUM` tokens come from.




{% highlight bison %}

%{
/*This is the beginning of code section 
that bison will insert to generated parser.
You can include here anything you need*/

#include <stdio.h>
%}

// union that will be used for storing 
// all necessary meta-information carried 
// by some tokens.
%union
{
    // this corresponds to yylval.ival 
    // from syntax_scanner.l file
    int ival; 
};
// Here are definitions of all tokens (terminals)
// Most of them don't carry any meta-information
// but only NUM carries ival along with itself
%token <ival> NUM
%token BEG
%token END
%token PLUS
%token MINUS
%token MUL
%token DIV

//Here is the list of rules (non-terminals)
//and their types. Not all rules have to be listed here.
//You can skip this part entirely if you don't
//want to return anything from any rule.
//We however will return integer from most of the rules.
%type<ival> Addition
%type<ival> Multiplication
%type<ival> Atomic

//Very important initial rule
%start Start

//Below you can find the definitions of all rules
%%
Start:
   Addition { 
       /*Here you can put all the code 
       that should be executed when 
       a given rule matches*/ 
       printf("Final result=%d\n",$1); 
   }
   | /*this matches empty string*/
   ;

Addition:
	Multiplication PLUS Addition { 
	 /**Here $$ is used to denote output of rule.
	  Under $1 you can find output of Multiplication.
	  Under $3 is the output of Addition.*/
	 $$ = $1 + $3; 
	}
	| Multiplication MINUS Addition { $$ = $1 - $3; }
	| Multiplication 
	;

Multiplication:
	Atomic MUL Multiplication { $$ = $1 * $3; }
	| Atomic DIV Multiplication {  $$ = $1 / $3; }
	| Atomic
	;

Atomic:
	NUM { $$ = $1; }
	| BEG Addition END  { $$ = $2; }
	;

%%

/** Here is some final code that can contain 
main functions and any other utilities. 
You can skip this section if want to 
keep it in a separate file */

int yyerror(char *s) {
  printf("yyerror : %s\n",s);
}

int main(void) {
  yyparse();
}

{% endhighlight %}

You may now run `./compile.sh` and `echo "4+3+5-5" | ./run.sh`, which should print 

    $ ./compile.sh
    ... lots of logs ...
    [100%] Built target first_compiler
    $ echo "4+3+5-5" | ./run.sh
    Final result=7
 

 
 
### Flex and Bison with AST
 
The above example works just fine but it's far from "real" compiler. It's basically just the parser. Let's generate AST, like professional compilers do. 


### Bad ideas and common beginner mistakes

#### Confusing AST with parse tree


I should point out that parse tree is something completely different from abstract syntax tree. It's very tempting to just define this


{% highlight C %}

struct Addition{
   /*0 for positive addition with '+', 
     1 for negative addition with '-' */
   int type; 
   struct * Multiplication this;
   struct * Addition next;
}
struct Multiplication{
   /*0 for multiplication with '*', 
     1 for division addition with '/' */
   int type; 
   struct * Atomic this;
   struct * Multiplication next;
}
struct Atomic{
   /* -1 for nested brackets '( )', 
     otherwise integer literal */
   int type; 
   struct * Addition nested;
}
{% endhighlight %}

and then you can represent `1+3*7` as

    \ Addition (type=0)
    |-- this=Multiplication (type=-1)
    |   |-- this=Atomic (type=1, nested=NULL)
    |   `-- next=NULL
    |-- next=Multiplication (type=0) 
        |-- this=Atomic (type=3, nested=NULL)
        `-- next=Multiplication (type=-1)
            |-- this=Atomic (type=7, nested=NULL)
            `-- next=NULL
        
The problem with this approach is that:

1. It's not any AST at all. This is a parse tree. It very often doesn't give you the same logical guarantees as AST and you will end up have to later convert it to AST anyway. If you are really stubborn you could theoretically use this as AST for all your purposes but you will then have to constantly check for corner cases when some values are NULL. All logical operations on such tree will double or quadruple number of cases that are necessary to check. It makes your coding more error-prone too.
2. Look how much space it wastes. And when performing recursion (which you will do really often on AST) space is an extremely valuable resource. It will cause you to later also waste stack space due to unnecessarily deep recursion levels of the data structure itself. Every level of extra recursion leads to more time being wasted on computing it.


#### Using arrays for "optimisation"

When you have to store addition of `1+1+1+1+1` as an AST it may seem like defining this

{% highlight C %}

struct AST{
   int type;
   int length;   
   union{
      struct * AST operands;
      int literal;
   }
}

{% endhighlight %}

is a good idea. In real-life however, you don't actually see long arrays of addition all that often. Usually the operations will be mixed together very often. This holds especially true for more complex languages where instead of 4 operations `-`, `+`, `*`, `/`, you have 10 or 20 of them. Then the likelyhood of this being any actual optimisation is nearly zero. The worst part is that you still need to pay the overhead of this "optimisation". 

1. First overhead comes from the fact that now you also need to store the length of array and probably  manage relocations and resizing. It's alot of heavyweight machinery that will slow you down more often than help. 
2. Second overhoad is much more dangerous - it's the increase in logical complexity of performing recursion of such data structures. You need to generalize all your induction to n-ary operations and suddenly a lot of new corner cases appear. This makes the development more error-prone.