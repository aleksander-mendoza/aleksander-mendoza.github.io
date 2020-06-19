---
layout: post
title:  "Compiler of Mealy automata"
date:   2020-06-06 01:07:00 +0200
categories: automata
permalink: /mealy_compiler.html
---
**Contents**
* TOC
{:toc}



In this post I will show you how to to extend the [Glushkov algorithm](/glushkov_compiler.html) and make it work with [regexes for functional Mealy machines](/mealy_regexes.html#special-case-of-functional-mealy-machines)

### Basics and sytnax

First let's make a short and more technical revise of our regex language. There is some set of allowed symbols _&Sigma;_. We also add empty string into the pool and obtain the set of atomic characters _&Sigma; &cup; {&epsilon;}_. In our regular expressions we will treat every _x &isin; &Sigma; &cup; {&epsilon;}_ as an atomic relation _x:&epsilon;_. So for instance, if we write a regex

    abc
    
we will mean by that _(a:&epsilon;)(b:&epsilon;)(c:&epsilon;)_ which in turn is equivalent to relation _(abc):&epsilon;_. Such regex will print empty string for input "abc" and reject all other inputs.

We allow operation of concatenation but there is no symbol for representing it. Sometimes some mathematicians would use dot &middot; to make it explicit. This would make our previous example look like _a&middot;b&middot;c = (a:&epsilon;)&middot;(b:&epsilon;)&middot;(c:&epsilon;) = (a&middot;b&middot;c):(&epsilon;&middot;&epsilon;&middot;&epsilon;) = (a&middot;b&middot;c):&epsilon;_. However, concatenation is such a common operation, that people usually ommit &middot; altogether.

Apart from concatenation, there is also union and Kleene closure. On UNIX it is often denoted with `|` but mathematicians usually write `+`. This way + and &middot; resemble addition and multiplication (and people who know abstract algebra will notice why it makes sense). We will go with + for union as well. Kleene closure is always denoted as `*`.

Lastly there is one more operation and this one is specific to Mealy machines. The language product `:` will be used for printing output. On the left hand side of `:` there can be any regular expression but on the right side we will always only put strings. Moreover the strings shall represent printed output and their characters might come from a different alphabet &Gamma;. Throughout this tutorial I will assume &Gamma;=&Sigma;, but should anybody be interested, I will make a tutorial in the future showing how to support distinct input and output alphabets. 

All in all, here is an example of our regular expressions

    abc:xyz*+ba(c+a):x

The Kleene closure `*` binds the strongest, then concatenation, then product `:`, and then union `+` is the weakest. However, remember that right side of `:` is not treated like a regular expression by itself. Instead we treat it as string in more "atomic" fashion. Therefore `abc:xyz*` should be read as `((abc):(xyz))*` instead of `(abc):(xy(z*))`. 

A grammar describing such expressions could be designed as

    Union ::= Product '+' Union | Product
    Product ::= Concat ':' Output | Concat
    Concat ::= Kleene Concat | Kleene
    Kleene ::= Atomic '*' | Atomic
    Atomic ::= SIGMA | EPSILON | '(' Union ')'
    Output ::= EPSILON | NonEmptyOutput
    NonEmptyOutput ::= SIGMA | SIGMA NonEmptyOutput

However, what if any of the key symbols `(`,`)`,`+`,`:`,`*` belongs to &Sigma;. This grammar would then be ambiguous. Also, what exactly is `EPSILON`? It's tedious and difficult to type &epsilon; on QWERTY keyboards. We could solve both problems with one solution, that is, by introducing quoted string literals. Epsilon could then be written as empty string `""` and we don't have to worry about conflicts with key symbols, because we can write `"()+:\""` to avoid confusion. On top of that, we could  simplify the grammar itself a little.

    Union ::= Product '+' Union | Product
    Product ::= Concat ':' QUOTED_STRING | Concat
    Concat ::= Kleene Concat | Kleene
    Kleene ::= Atomic '*' | Atomic
    Atomic ::= QUOTED_STRING | '(' Union ')'

At this point there is one last improvement to be made. Observe that this grammar forces us to write

    ("abc":"x") ("ab":"y")

but it would be more pleasing to drop those brackets, since quoted strings already work a bit like brackets by themselves. Unfortunately current grammar interprets `"abc":"x" "ab":"y"` as `(("abc"):"x" "ab"):"y"`. In this particular case we are lucky and both expressions are equivalent, but more complicated examples might lead to problems. So in order to take advantage of bracket-like behaviour of quotes `"`, let's swap the binding strength:

    Union ::= Concat '+' Union | Concat
    Concat ::= Kleene Concat | Kleene
    Kleene ::= Product '*' | Product
    Product ::= Atomic ':' QUOTED_STRING | Atomic
    Atomic ::= QUOTED_STRING | '(' Union ')'
    
    
### Parser in Flex and Bison

The file structure used in this tutorial is the same as in [the quick compiler tutorial](/compiler_in_c.html). Let's get started with this basic code for the parser itslef.

The scanner `syntax_scanner.l` is a slight modification of the one from [previous post](/glushkov_compiler.html):

{% highlight flex %}

%{ 
#include "syntax_parser.h" 
%}
letter          [^():\\+*]
string_start    ["]([^"\\]|\\.)*

%option noyywrap  
%%

"+"	    return(PLUS); 
"("	    return(BEG);
")"	    return(END);
":"	    return(COLON);
"*"	    return(STAR);
{string_start}["]   { int len = strlen(yytext);
                      yylval.strval = malloc(len-2);
                      for(int i=0;i<len-2;i++){
                        yylval.strval[i] = yytext[i+1]; 
                      }
                      return STRING; }
{string_start}      { /* Register unterminated string error */ }
{% endhighlight %}

The most notable change is addition of `string_start` which matches double-quoted string literals.

Our abstract syntax tree is now defined in `main.h` as

{% highlight C %}
#ifndef H_MAIN
#define H_MAIN
struct AST;

struct ASTLetter{
	char literal;
};

struct ASTUnion{
	struct AST * lhs;//left-hand side
	struct AST * rhs;//right-hand side
};

struct ASTConcat{
	struct AST * lhs;
	struct AST * rhs;
};

struct ASTKleene{
	struct AST * child;
};

struct ASTOutput{
	struct AST * child;
	char * outStr;
};

struct AST{
	/**0 for union;1 for concatenation; 
	2 for kleene closure; 3 for printing output; 
	4 for letter literal; 5 for epsilon literal*/
	int type;
	union{
		struct ASTUnion uni;
		struct ASTConcat concat;
		struct ASTKleene kleene;
		struct ASTOutput output;
		struct ASTLetter letter;
	};
};

struct AST * mkUnion(struct AST * lhs,struct AST * rhs);
struct AST * mkConcat(struct AST * lhs,struct AST * rhs);
struct AST * mkKleene(struct AST * child);
struct AST * mkOutput(struct AST * child,char * strOut);
struct AST * mkLetter(char literal);
struct AST * mkEpsilon();

#endif
{% endhighlight %}

The most notable extension is the addition of `ASTOutput` as well as using `type=5` to encode epsilons. This will be of great importance for Glushkov's algorithm. I will explain it in more detail below. The general idea is to scan `string_start` in flex, but then each string should be decomposed in Bison into concatenation of indivitual letters. For instance, if regular expression is `"abc"` then `string_start` will produce `STRING` token with `yylval.strval="abc"` (Notice that the quotes are stripped away at this point. Therefore we don't end up with  `yylval.strval="\"abc\""`). Later in Bison we will produce the following AST:

    mkConcat(
      mkConcat(
        mkConcat(
          mkEpsilon(),
          mkLetter('a')
        ),
        mkLetter('b')
      ),
      mkLetter('c')
    )
    
The exact code for `syntax_parser.y` should look as follows


{% highlight bison %}

%{

#include <stdio.h>
#include "main.h"
#include <stdlib.h>
void printSpaces(int indentation){
    for(int i=0;i<indentation;i++){
        printf(" ");
    }
}
void inductivePrint(struct AST * root,int indentation){
    printSpaces(indentation);
    switch(root->type){
    case 0:
        printf("+\n");
        inductivePrint(root->uni.lhs,indentation+2);
        inductivePrint(root->uni.rhs,indentation+2);
        return;
    case 1:
        printf(".\n");
        inductivePrint(root->concat.lhs,indentation+2);
        inductivePrint(root->concat.rhs,indentation+2);
        return;
    case 2:
        printf("*\n");
        inductivePrint(root->kleene.child,indentation+2);
        return;
    case 3:
        printf(":%s\n",root->output.outStr);
        inductivePrint(root->output.child,indentation+2);
        return;
    case 4:
        printf("%c\n", root->letter.literal);
        return;
    case 5:
        printf("epsilon\n");
        return;
    }
}

%}

%union
{
    char * strval;
    struct AST * ast;
};

%token <strval> STRING
%token BEG
%token END
%token PLUS
%token STAR
%token COLON

%type<ast> Union
%type<ast> Concat
%type<ast> Kleene
%type<ast> Letter
%type<ast> Output

%start Start

%%
Start:
    Union {
        inductivePrint($1,0);
    }
    | 
    ;

Union:
    Concat PLUS Union { $$ = mkUnion($1,$3); }
    | Concat {$$ = $1;}
    ;

Concat:
    Kleene Concat { $$ = mkConcat($1,$2); }
    | Kleene {$$ = $1;}
    ;

Kleene: 
    Output {$$ = $1;}
    | Output STAR { $$ = mkKleene($1); }
    ;

Output: 
    Letter COLON STRING { $$ = mkOutput($1, $3);}
    | Letter { $$ = $1; }
    ;

Letter:
    STRING { 
        $$ = mkEpsilon();
        for(int i=0;($1)[i]!=0;i++){
            $$ = mkConcat($$,mkLetter(($1)[i])); 
        }
    }
    | BEG Union END  { $$ = $2; }
    ;

%%

int yyerror(char *s) {
  printf("yyerror : %s\n",s);
}

int main(void) {
  yyparse();
}
{% endhighlight %}

The `Letter` token might be a bit confusing. It matches entire `STRING` instead of single letters. However it later decomposes the string in a for-loop, producing atomic letter as a result. Hence the name `Letter`. Notice that we only decompose input strings, but we don't do the same for `Output`. In `mkOutput($1, $3)` we pass `$3` straight as `char *`. This works, because we configured Bison to use  `%token <strval> STRING`.

Lastly, the contents of `main.c` with all the helper functions are defined as follows:


{% highlight C %}

#include <stdlib.h>
#include "main.h"

struct AST * mkUnion(struct AST * lhs,struct AST * rhs){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 0;
	struct ASTUnion uni = {lhs,rhs};
	node->uni = uni;
	return node;
}
struct AST * mkConcat(struct AST * lhs,struct AST * rhs){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 1;
	struct ASTConcat concat = {lhs,rhs};
	node->concat = concat;
	return node;
}
struct AST * mkKleene(struct AST * child){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 2;
	struct ASTKleene kleene = {child};
	node->kleene = kleene;
	return node;
}
struct AST * mkOutput(struct AST * child,char * strOut){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 3;
	struct ASTOutput output = {child,strOut};
	node->output = output;
	return node;
}
struct AST * mkLetter(char c){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 4;
	struct ASTLetter letter = {c};
	node->letter = letter;
	return node;
}
struct AST * mkEpsilon(){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 5;
	return node;
}

{% endhighlight %}

This code should work just fine and compile correctly as it is. You may use `./compile.sh` (defined [here](/compiler_in_c.html)) followed by

{% highlight Bash %}
$ echo '"rete":"ter"+"re":"rtre"' | ./run.sh 

+
  :ter
    .
      .
        .
          .
            epsilon
            r
          e
        t
      e
  :rtre
    .
      .
        epsilon
        r
      e
{% endhighlight %}

### Glushkov's algorithm for functional Mealy machines

The standard version of Glushkov's algorithm has 3 phases: first the regular expression is linearized, then using recursion we analyse the syntax tree and produce sets _B_, _E_ and _L_ that describe a local language, and lastly we use _B_, _E_ and _L_ to build a finite state automaton. You can look for more details in [previous post](/glushkov_compiler.html#implementing-glushkovs-algorithm). Here I will show you a modified version of this algorithm. First phase stays the same but the recursive functions need to be vastly reworked.

Let _A_ be a function that takes a regular expression formula and returns set of outputs that the formula would print if it matched empty string &epsilon;. The type can be described as _A : AST &rarr; 2<sup>&Gamma;*</sup>_. In our particular case we assume &Sigma; and &Gamma; to be ASCII letter, hence we get the following examples:




<table>
<thead>
  <tr>
    <th markdown="span"> formula _&phi;=_ </th>
    <th markdown="span">_A(&phi;)=&phi;(&epsilon;)=_</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td markdown="span">`"a":"c"*`</td>
    <td markdown="span">_{&epsilon;}_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"abc"`</td>
    <td markdown="span">_{abc}_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"c"*`</td>
    <td markdown="span">_{&epsilon;,c,cc,ccc...}_</td>
  </tr>
  <tr>
    <td markdown="span">`"a":"c"`</td>
    <td markdown="span">_&empty;_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"c" + "":"b"`</td>
    <td markdown="span">_{c,b}_</td>
  </tr>
</tbody>
</table>

Hence we obtain the following recursive definition of _A_.


> A(x+y) = A(x) &cup; A(y)  
> A(xy) = A(x) A(y)  
> A(x:y) = A(x) {y}  
> A(x*) = A(x)\*   
> A(&epsilon;) = {&epsilon;}  
> A(x &isin; &Sigma;') = &empty;  

I deliberately used &Sigma;' instead of &Sigma; to highlight the fact that we work on linearized alphabet (&Sigma; was the original alphabet, but then we renamed all leters in formula &phi;, so that they became unique. This yields a new alphabet &Sigma;'). Moreover, there are a couple of things worth clarifying. First of all, remember  that _A(x) A(y)_ stands for concatenation of sets of strings (a.k.a formal languages). The same goes for _A(x) {y}_. For instance concatenation of _{a,b,c}_ and _{0,1,2}_ yields _{a0,a1,a2,b0,b1,b2,c0,c1,c2}_. The empty set has a special property that _&empty; X = X &empty; = &empty;_ for any set X&sub;&Sigma;'. Kleene closure used in _A(x)\*_, works analogically. For instance _{a,b}*={&epsilon;,a,b,aa,ab,bb,ba,aaa,aab,abb,aba,...}_. 

Since we are dealing with functional Mealy machines, also we need to make sure that \|A(&phi;)\|&le;1. If this condition is not met, then we should report an error and abort the algorithm.


Next we need to define _B_ as a function that collects all linearized symbols &Sigma;' that could be found at the beginning of some string accepted by formula &phi;. It also collects all output printed before reading those symbols. Type of this function is _B:AST &rarr; 2<sup>&Sigma;' &times; &Gamma;*</sup>_. For instance

<table>
<thead>
  <tr>
    <th markdown="span"> formula _&phi;=_ </th>
    <th markdown="span">_A(&phi;)=&phi;(&epsilon;)=_</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td markdown="span">`"a":"c"*`</td>
    <td markdown="span">_{ (a,{&epsilon;}) }_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"abc"`</td>
    <td markdown="span">_&empty;_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"c"*`</td>
    <td markdown="span">_&empty;_</td>
  </tr>
  <tr>
    <td markdown="span">`"ab":"c"`</td>
    <td markdown="span">_{ (a,{&epsilon;}) }_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"x" "a":"y"`</td>
    <td markdown="span">_{ (a,{x}) }_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"xx" "ca" + "":"y" "be"`</td>
    <td markdown="span">_{ (c,{xx}), (b,{y}) }_</td>
  </tr>
  
</tbody>
</table>

We need to introduce a special operation that will facilitate writing further definitions. Given some set _X &sub; &Sigma;' &times; &Gamma;*_ and _Y &sub; &Gamma;*_ the concatenation _X Y_ should be understood as concatenation of _&Gamma;*_ component of _X_ with _Y_ and yielding a new set _X Y &sub; &Sigma;' &times; &Gamma;*_. For instance if _X={(a,b), (c,d)}_ and _Y={e,f}_ then _X Y={(a,be), (a,bf), (c,de), (c,df)}_. Analogically for _Y X = {(a,eb), (a,fb), (c,ed), (c,fd)}_. 
Now _B_ can be defined as follows:

> B(x+y) = B(x)&cup;B(y)   
> B(xy) = B(x)&cup; ( A(x) B(y) )   
> B(x:y) = B(x)  
> B(x*) = B(x)    
> B(&epsilon;) = &empty;  
> B(x &isin; &Sigma;') = {(x,&epsilon;)}

Next we need to create function _E: AST &rarr; 2<sup>&Sigma;' &times; &Gamma;*</sup>_ which returns all symbols that could be found at the end of recognized strings. For example

<table>
<thead>
  <tr>
    <th markdown="span"> formula _&phi;=_ </th>
    <th markdown="span">_A(&phi;)=&phi;(&epsilon;)=_</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td markdown="span">`"a":"c"*`</td>
    <td markdown="span">_{ (a,{c}) }_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"abc"`</td>
    <td markdown="span">_&empty;_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"c"*`</td>
    <td markdown="span">_&empty;_</td>
  </tr>
  <tr>
    <td markdown="span">`"ab":"c"`</td>
    <td markdown="span">_{ (b,{c}) }_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"x" "a":"y"`</td>
    <td markdown="span">_{ (a,{y}) }_</td>
  </tr>
  <tr>
    <td markdown="span">`"":"xx" "ca":"kk" + "":"y" "be":"l"`</td>
    <td markdown="span">_{ (a,{kk}), (e,{l}) }_</td>
  </tr>
  
</tbody>
</table>

> E(x+y) = E(x)&cup;E(y)    
> E(xy) = ( E(x) A(y) ) &cup; E(y)   
> E(x:y) = E(x) {y}    
> E(x*) = E(x)    
> E(&epsilon;) = &empty;   
> E(x &isin; &Sigma;') = {(x,&epsilon;)}   

At this point, we should take a moment to admire one special property of our grammar. This definition

    Product ::= Atomic ':' QUOTED_STRING | Atomic

allow us to use singleton set in the case of

> E(x:y) = E(x) {y}  

Moreover from the the fact that _&phi;_  has been linearized and uses unique symbols from _&Sigma;'_ we can deduce that the lines

> E(x+y) = E(x)&cup;E(y)    
> E(xy) = ( E(x) A(y) ) &cup; E(y)   

give us guarantee of E(x) and E(y) being disjoint (analogically for _B_). Since they are always disjoin and _\|A(y)\|&le;1_ and _{y}_ is singleton, we can imply that both _B(&phi;)  &sub; &Sigma;' &times; &Gamma;*_ and _E(&phi;) &sub; &Sigma;' &times; &Gamma;\*_ are actually functions _B(&phi;)  &sub; &Sigma;' &rarr; &Gamma;\*_ and _E(&phi;) &sub; &Sigma;' &rarr; &Gamma;\*_.  
More intuitively consider the example of _&phi;=_`"a":"x" + "a":"y"`, which yields _E(&phi;)={ (a,{x,y}) }_. Such subset of _&Sigma;' &times; &Gamma;*_ is not a function _&Sigma;' &rarr; &Gamma;*_ because it returns multiple values for _a_. If we linearize &phi; as `"0":"x" + "1":"y"` then _E(&phi;)={ (0,{x}) (1,{y}) }_ becomes a function. All this will be crucial later.

Finally the last function is _L_ which collects all possible substrigs of length 2. Each such substring _xy &isin; &Sigma;'&Sigma;'_ will represent a transition from _x_ to _y_. All elements of _&Sigma;'_ will later become states of automaton. We also need to keep track of output that should be printed at the transition from _x_ to _y_. The type of _L_ is therefore _L:AST &rarr; &Sigma;'&Sigma;'&times;&Gamma;\*_

> L(x+y) = L(x)&cup;L(y)    
> L(xy) = E(x)B(y) &cup; L(x) &cup; L(y)     
> L(x:y) = L(x)  
> L(x*) = E(x)B(x) &cup; L(x)  
> L(&epsilon;) = &empty;     
> L(x &isin; &Sigma;') = &empty;  

Just like previously we gave special meaning to concatenation of _E(x) A(y)_ or  _E(x) {y}_, the same is true here for concatenations of _E(x)B(y)_ and _E(x)B(x)_. We separately concatenate components _&Sigma;'_ and _&Gamma;\*_ of _&Sigma;'&times;&Gamma;\*_. For instance given 
_X = {(a,{x,y}), (b,{z})}&isin;&Sigma;'&times;&Gamma;\*_ and _Y = {(q,{0,1}), (r,{2})}&isin;&Sigma;'&times;&Gamma;\*_ their conctanetation is _X Y = {(aq,{x0,y0,x1,y1}), (ar,{x2,y2}), (bq,{z0,z1}), (br,{z2})}&isin;&Sigma;'&Sigma;'&times;&Gamma;\*_. 

Again, notice that because _B_ and _E_ always yield functions _&Sigma;' &rarr; &Gamma;\*_, the _L_ will also yield functions _L(&phi;)&sub;&Sigma;'&Sigma;'&rarr;&Gamma;\*_.

Having all of the definitions for _A_, _B_, _E_ and _L_ ready, we can now move on to third phase of the algorithm. We need to turn _B_, _E_ and _L_ into automaton. We shall use a very specific model of Mealy machine, that is, the [hybrid Mealy-Moore machine](/transducers_intuitively.html#hybrid-of-mealy-moore-automata).

All symbols from _&Sigma;'_ should be treated as states of the machine. Additionally we include _&epsilon;_ as the unique initial state. Hence the set of states _Q_ equals _Q=&Sigma;' &cup; {&epsilon;}_. We shall put a transition between _x,y&isin;&Sigma;'&sub;Q_ whenever _(xy,z) &isin; L(&phi;)_ with _z_ being the transition output. We also put transitions from _&epsilon; &isin; Q_ to the state _x &isin; &Sigma;'_ for all those _x_'es such that _(x,z) &isin; B(&phi;)_. The Moore-style outputs for each state are determined by _E(&phi;)_. That is, if _(x,z) &isin; E(&phi;)_ then the state _x_ will be an accepting state which prints Moore-style output _z_. Lastly we add Moore-style output to state _&epsilon;_ if _z&isin;A(&phi;)_.


### C optimization of Glushkov's algorithm


Now, that we know the theory behind Glushkov's construction, we can try to optimize it and build C version of it.