---
layout: post
title:  "Quick compiler tutorial (C version)"
date:   2020-05-27 01:07:00 +0200
categories: compilers
permalink: /compiler_in_c.html
---

**Contents**
* TOC
{:toc}

### Making compilers is easy
People often think of compiler design as sort of black magic. Some arcane craft known by very few. In reality it's far simpler than that.  It's all about languages and as we all know, languages are the essence of computer science. A compiler is nothing more than just translator for machines. You can make a compiler that converts Turing machines to lambda calculus, or string rewrite systems to recursive functions. In one of the [previous posts](/compiling_regexes.html) I already explained how to translate regular expressions to finite state machines. That was in essence a compiler. 

### Toy language

The end goal of this tutorial is creating a very simple parser for a toy language, which can evaluate arithmetic expressions like for example

    1+2/4*6+(1+3*(-4))
    
    
If you succeed at finishing this tutorial, then you will be albe to move on to a more ambitious project. 

### Setup

This tutorial will show you how to make your first compiler in C with flex&bison. For convenience I will use CMake as build system. 

    /
    |-- src/
    |   |-- main.c
    |   |-- syntax_parser.y
    |   |-- syntax_scanner.l
    |   |-- CMakeLists.txt
    |   `-- include/
    |       `-- main.h
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
    include/main.h
    ${BISON_SyntaxParser_OUTPUTS}
    ${FLEX_SyntaxScanner_OUTPUTS}
)
# Add directories to include
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

If we (sucessfully) compile our C project, we should be able to run our compiler with this `run.sh` script:

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
picture on the left represents the following bracketing: `1+(2+(3+4))`. The result of addition would still be the same even if bracketing was `(1+2)+(3+4)`, but imagine what would happen iff instead of addition it was division `1/2/3/4`. The order of operations will make huge difference in this case.

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
 
The above example works just fine but it's far from "real" compiler. It's basically just the parser. Let's generate AST, like professional compilers do. First put this in `main.h`.

{% highlight C %}
#ifndef H_MAIN
#define H_MAIN
struct AST;

struct ASTAtomic{
	int literal;
};

struct ASTPlus{
	struct AST * lhs;//left-hand side
	struct AST * rhs;//right-hand side
};

struct ASTMinus{
	struct AST * lhs;
	struct AST * rhs;
};

struct ASTMult{
	struct AST * lhs;
	struct AST * rhs;
};

struct ASTDiv{
	struct AST * lhs;
	struct AST * rhs;
};

struct AST{
	/**0 for plus;1 for minus; 
	2 for mult; 3 for div;
	4 for atom*/
	int type;
	union{
		struct ASTPlus plus;
		struct ASTMinus minus;
		struct ASTMult mult;
		struct ASTDiv div;
		struct ASTAtomic atom;
	};
};

struct AST * mkPlus(struct AST * lhs,struct AST * rhs);
struct AST * mkMinus(struct AST * lhs,struct AST * rhs);
struct AST * mkMult(struct AST * lhs,struct AST * rhs);
struct AST * mkDiv(struct AST * lhs,struct AST * rhs);
struct AST * mkAtom(int literal);
#endif
{% endhighlight %}

This is our definition of abstract syntax tree. I recommend reading [this post](/understanding_programming.html) in order to better understand this code. Essentially this is nothing more than a recursive definition of arithemtic expression:

> 1. every number between 0 (inclusive) and &infin; (exclusive) is an arithmetic expression
> 2. if _x_ and _y_ are arithemtic expressions, then _x+y_ is an arithemtic expression.
> 3. if _x_ and _y_ are arithemtic expressions, then _x-y_ is an arithemtic expression.
> 4. if _x_ and _y_ are arithemtic expressions, then _x*y_ is an arithemtic expression.
> 5. if _x_ and _y_ are arithemtic expressions, then _x/y_ is an arithemtic expression.

In our code `ASTAtomic` corresponds to _1<sup>st</sup>_ case of the inductive definition. Similarly `ASTPlus` corresponds to _2<sup>nd</sup>_, `ASTMinus` to _3<sup>rd</sup>_, `ASTMult` to _4<sup>th</sup>_ and `ASTDiv` to _5<sup>th</sup>_. Therefore every arithemtic expression is either 1<sup>st</sup>,2<sup>nd</sup>,3<sup>rd</sup>,4<sup>th</sup> or 5<sup>th</sup> case. This can be represented with the struct `AST` which can either contain `ASTAtomic`, `ASTPlus`, `ASTMinus`, `ASTMult` or `ASTDiv`. 

I also added a couple of `mkXXXX` functions for facility. The implementation should be stored in `main.c`:


{% highlight C %}
#include <stdlib.h>
#include "main.h"

struct AST * mkPlus(struct AST * lhs,struct AST * rhs){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 0;
	struct ASTPlus plus = {lhs,rhs};
	node->plus = plus;
	return node;
}
struct AST * mkMinus(struct AST * lhs,struct AST * rhs){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 1;
	struct ASTMinus minus = {lhs,rhs};
	node->minus = minus;
	return node;
}
struct AST * mkMult(struct AST * lhs,struct AST * rhs){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 2;
	struct ASTMult mult = {lhs,rhs};
	node->mult = mult;
	return node;
}
struct AST * mkDiv(struct AST * lhs,struct AST * rhs){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 3;
	struct ASTDiv div = {lhs,rhs};
	node->div = div;
	return node;
}
struct AST * mkAtom(int literal){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 4;
	struct ASTAtomic atom = {literal};
	node->atom = atom;
	return node;
}

{% endhighlight %}


Now we should change our bison code (`syntax_parser.y`) as follows:


{% highlight bison %}

%{

#include <stdio.h>
#include "main.h"

/**Use the inductive definition of 
AST to evaluate the expression*/
int inductiveEval(struct AST * root){
    switch(root->type){
    case 0:
        return inductiveEval(root->plus.lhs)+
           inductiveEval(root->plus.rhs);
    case 1:
        return inductiveEval(root->minus.lhs)-
           inductiveEval(root->minus.rhs);
    case 2:
        return inductiveEval(root->mult.lhs)*
           inductiveEval(root->mult.rhs);
    case 3:
        return inductiveEval(root->div.lhs)/
           inductiveEval(root->div.rhs);
    case 4:
        return root->atom.literal;
    }
}

%}

%union
{
    int ival; 
    struct AST * ast;
};

%token <ival> NUM
%token BEG
%token END
%token PLUS
%token MINUS
%token MUL
%token DIV

%type<ast> Addition
%type<ast> Multiplication
%type<ast> Atomic

%start Start

%%
Start:
   Addition { printf("Final result=%d\n",inductiveEval($1));}
   | 
   ;

Addition:
	Multiplication PLUS Addition { $$ = mkPlus($1,$3); }
	| Multiplication MINUS Addition { $$ = mkMinus($1,$3); }
	| Multiplication {$$ = $1;}
	;

Multiplication:
	Atomic MUL Multiplication { $$ = mkMult($1,$3); }
	| Atomic DIV Multiplication {  $$ = mkDiv($1,$3); }
	| Atomic {$$ = $1;}
	;

Atomic:
	NUM { $$ = mkAtom($1); }
	| BEG Addition END  { $$ = $2; }
	;

%%

int yyerror(char *s) {
  printf("yyerror : %s\n",s);
}

int main(void) {
  yyparse();
}

{% endhighlight %}

Here we no longer evaluate the expression on the fly as we parse. Instead we first build AST and then use induction to evaluate it.
You may now run 

    $echo "4+3+5-5" | ./run.sh
    Final result=7
  
and see that everything works as it worked before. However, this time thanks to AST you can do much more than just one-time evalutation. You can add even more inductive definitions. For example here is code that prints the AST:

{% highlight C %}
printSpaces(int indentation){
    for(int i=0;i<indentation;i++){
        printf(" ");
    }
}
void inductivePrint(struct AST * root, int indentation){
    printSpaces(indentation);
    switch(root->type){
    case 0:
        printf("+\n");
        inductivePrint(root->plus.lhs,indentation+4);
        inductivePrint(root->plus.rhs,indentation+4);
        return;
    case 1:
        printf("-\n");
        inductivePrint(root->minus.lhs,indentation+4);
        inductivePrint(root->minus.rhs,indentation+4);
        return;
    case 2:
        printf("*\n");
        inductivePrint(root->mult.lhs,indentation+4);
        inductivePrint(root->mult.rhs,indentation+4);
        return;
    case 3:
        printf("/\n");
        inductivePrint(root->div.lhs,indentation+4);
        inductivePrint(root->div.rhs,indentation+4);
        return;
    case 4:
        printf("%d\n",root->atom.literal);
        return;
    }
}
{% endhighlight %}

You can then use it analogically to `inductiveEval`:

{% highlight bison %}
Start:
   Addition { inductivePrint($1,0);}
   | 
   ;

{% endhighlight %}

If you compile and run you should see a tree in this format:

    $ echo "4+3+5-5" | ./run.sh 
    +
        4
        +
            3
            -
                5
                5
                

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