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

Finally the last function is _L_ which collects all possible substrigs of length 2. Each such substring _xy &isin; &Sigma;'&Sigma;'_ will represent a transition from _x_ to _y_. All elements of _&Sigma;'_ will later become states of automaton. We also need to keep track of output that should be printed at the transition from _x_ to _y_. The type of _L_ is therefore _L:AST &rarr; 2<sup>&Sigma;'&Sigma;'&times;&Gamma;\*</sup>_

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


Now, that we know the theory behind Glushkov's construction, we can try to optimize it and build C version of it. First of all, having all those separate functions _A_, _B_, _E_ and _L_ is not very efficient, because _L_ calls _B_ and _E_, which in turn call _A_ multiple times on the same input, leading to plenty of redundantly duplicated computation. We can merge all those functions together into just one function _f_ that returns all outputs of _L_, _B_, _E_, _A_ in a tuple _f:AST &rarr; 2<sup>&Sigma;'&Sigma;'&times;&Gamma;\*</sup>&times;2<sup>&Sigma;'&times;&Gamma;\*</sup>&times;2<sup>&Sigma;'&times;&Gamma;\*</sup>&times;2<sup>&Gamma;\*</sup>_. Therefore _f_ is equivalent to _f(&phi;) = (L(&phi;),B(&phi;),E(&phi;),A(&phi;))_. It can be defined inductively as:
 

> f(x+y) = (l<sub>x</sub>&cup; l<sub>y</sub>, b<sub>x</sub>&cup; b<sub>y</sub>, e<sub>x</sub>&cup; e<sub>y</sub>, a<sub>x</sub>&cup; a<sub>y</sub>) where f(x)=(l<sub>x</sub>,b<sub>x</sub>,e<sub>x</sub>,a<sub>x</sub>) and f(y)=(l<sub>y</sub>,b<sub>y</sub>,e<sub>y</sub>,a<sub>y</sub>)   
> f(xy) = (e<sub>x</sub>b<sub>y</sub>&cup; l<sub>x</sub>&cup; l<sub>y</sub>, b<sub>x</sub>&cup; (a<sub>x</sub> b<sub>y</sub>), (e<sub>x</sub> a<sub>y</sub>)&cup; e<sub>y</sub>, a<sub>x</sub>a<sub>y</sub>) where f(x)=(l<sub>x</sub>,b<sub>x</sub>,e<sub>x</sub>,a<sub>x</sub>) and f(y)=(l<sub>y</sub>,b<sub>y</sub>,e<sub>y</sub>,a<sub>y</sub>)    
> f(x:y) = (l<sub>x</sub>, b<sub>x</sub>, e<sub>x</sub> {y}, a<sub>x</sub> {y})  where f(x)=(l<sub>x</sub>,b<sub>x</sub>,e<sub>x</sub>,a<sub>x</sub>)   
> f(x*) = (e<sub>x</sub>b<sub>x</sub>&cup; l<sub>x</sub>, b<sub>x</sub> ,e<sub>x</sub>, a<sub>x</sub>\*)  where f(x)=(l<sub>x</sub>,b<sub>x</sub>,e<sub>x</sub>,a<sub>x</sub>)  
> f(&epsilon;) = (&empty;,&empty;,&empty;,{&epsilon;})  
> f(x &isin; &Sigma;') = (&empty;,{(x,&epsilon;)},{(x,&epsilon;)},&empty;)

Now we can start to write the C code for _f_. The greatest challenge is choosing the right representation for sets of strings. I want to keep things as simple as possible, therefore I will just arrays. In one of the [previous posts](/understanding_programming.html#other-data-structures) I explained that arrays can be though of as partial functions from _&#8469;_ to some set. We also know that _&phi;_ has been linearized and uses unique symbols from _&Sigma;'_, therefore we could enumerate all those symbols and use them as indices in array _&Sigma;'&sub;&#8469;_. I also explained above why, in case of functional Mealy machines, the set _2<sup>&Sigma;'&times;&Gamma;\*</sup>_ should be in fact treated as a partial function _&Sigma;'&#8640;&Gamma;\*_. All this leads us to representing _f_ as
_f:AST &rarr; (&Sigma;'&Sigma;'&#8640;&Gamma;\*)&times;(&Sigma;'&#8640;&Gamma;\*)&times;(&Sigma;'&#8640;&Gamma;\*)&times;2<sup>&Gamma;\*</sup>_. Since _&Sigma;'&sub;&#8469;_ we obtain partial functions _f:AST &rarr; (&#8469;&times;&#8469;&#8640;&Gamma;\*)&times;(&#8469;&#8640;&Gamma;\*)&times;(&#8469;&#8640;&Gamma;\*)&times;2<sup>&Gamma;\*</sup>_. Notice how I decided to represent  _&Sigma;'&Sigma;'_ as a tuple _&Sigma;'&times;&Sigma;'_, which then became _&#8469;&times;&#8469;_. The functions _&#8469;&#8640;&Gamma;\*_ can be represented in C as arrays of strings `char*[]`. The function _&#8469;&times;&#8469;&#8640;&Gamma;\*_ is equivalent to 2D array `char*[][]`. This brings us to defining _f:AST &rarr; `char*[][]`&times;`char*[]`&times;`char*[]`&times;2<sup>&Gamma;\*</sup>_. The last element standing on our way to pure C definition is the set _2<sup>&Gamma;\*</sup>_. We should remember that since we are working with functional Mealy machines, we have the guarantee of \|A(&phi;)\|&le;1, hence we can be sure that _2<sup>&Gamma;\*</sup>_ is either empty or a singleton set. [In this post](/understanding_programming.html#optional-types) I explained that such sets are often known as optional types and in C one can represent them with nullable pointers. For this reason, _2<sup>&Gamma;\*</sup>_ will become `char*` and we will use `NULL` to indicate empty set &empty;. This brings us to _f:AST &rarr; `char*[][]`&times;`char*[]`&times;`char*[]`&times;`char*`_. You should remember that [tuples can be represented in C using structs](/understanding_programming.html#products-unions-functions). Therefore we obtain

{% highlight C %}
struct T{
    char*** l; // char*[][]
    char** b; // char*[]
    char** e; // char*[]
    char* a;
};
{% endhighlight %}

and `char*[][]`&times;`char*[]`&times;`char*[]`&times;`char*`=`T`, which gives us _f:AST &rarr; `T`_. Finally we reached a pure C definition of function, which becomes

{% highlight C %}
T f(AST * ast){...} // we can hope that C compiler will perform return-by-copy elision.
{% endhighlight %}

Before I give you the actual code for _f_, let's step back a little. We don't have code for linearization yet. 


### C implementation of Glushkov's algorithm

First we need to linearize the alphabet. This could be done by traversing AST and pushing all atomic letters onto stack. As a result we would obtain an array that assigns unique index to each letter.

{% highlight C %}
int localize(struct AST * root, int offset, char * stack){
    switch(root->type){
    case 0:
        return localize(root->uni.rhs,
            localize(root->uni.lhs,offset,stack),stack);
    case 1:
        return localize(root->concat.rhs,
            localize(root->concat.lhs,offset,stack),stack);
    case 2:
        return localize(root->kleene.child,offset,stack);
    case 3:
        return localize(root->output.child,offset,stack);
    case 4:
        stack[offset] = root->letter.literal;
        root->letter.literal = (char)offset;
        return offset+1;
    case 5:
        return offset;
    }
}
{% endhighlight %}

As we push each letter onto stack `stack[offset] = root->letter.literal` we simultaneously swap it with its index `oot->letter.literal = (char)offset`. This way the AST will now contain unique letters. There is only one problem. We don't know ahead of time how large `char * stack` to allocate. We should first count all occurences of atomic letters (ignoring &epsilon;).

{% highlight C %}
int count(struct AST * root){
    switch(root->type){
    case 0:
        return count(root->uni.lhs)+count(root->uni.rhs);
    case 1:
        return count(root->concat.lhs)+count(root->concat.rhs);
    case 2:
        return count(root->kleene.child);
    case 3:
        return count(root->output.child);
    case 4:
        return 1;
    case 5:
        return 0;
    }
}
{% endhighlight %}

Notice that when performing linearization, we leave &epsilon; as is. Our version of glushkov's algorithm, makes extensive use of &epsilon;'s and we should not lose them! They won't be included as states in produced automaton anyway (in fact, the resulting automaton will be &epsilon;-free on input side).  

Having all this, we can move on and implement several utility functions vital for constructing _f_. As I said before, we gave special meaning concatenation _E(x)B(y)_ in

> L(xy) = E(x)B(y) &cup; L(x) &cup; L(y)  

or to _E(x) A(y)_ in

> E(xy) = ( E(x) A(y) ) &cup; E(y)  

Now we need to implement that special meaning in C. First of all, there is the singleton set concatenation:

{% highlight C %}
char * concat(char * x,char * y){
    if(x==NULL || y==NULL)return NULL;
    char * n = malloc(sizeof(char)*(strlen(x)+strlen(y)+1));
    strcpy(n,x);
    strcat(n,y);
    return n;
}
{% endhighlight %} 
 
Because `char * x` and `char * y` are pointers, they might be `NULL`, which represents set &empty;. If they are not null then they represent singleton set containing only one string. Having this function, we can now implement concatenation like in _E(x) A(y)_.

{% highlight C %}
/**Concatenate arbitrary set with signleton set*/
char** setConcatStr(char ** x,int size,char * y){
    char ** n = malloc(sizeof(char*)*size);
    for(int i = 0;i<size;i++){
        n[i] = concat(x[i],y);
    }
    return n;
}
/**Concatenate signleton set with arbitrary set*/
char** strConcatSet(char * x,int size,char ** y){
    char ** n = malloc(sizeof(char*)*size);
    for(int i = 0;i<size;i++){
        n[i] = concat(x,y[i]);
    }
    return n;
}
{% endhighlight %}

Next we can implement concatenations of two arbitrarily-sized sets:

{% highlight C %}
char*** concatProd(char ** x,char ** y,int size){
    char *** n = malloc(sizeof(char**)*size);
    for(int i = 0;i<size;i++){
        n[i] = malloc(sizeof(char*)*size);
    }
    for(int i = 0;i<size;i++){
        for(int j = 0;j<size;j++){
            n[i][j] = concat(x[i],y[j]);
        }
    }
    return n;
}
{% endhighlight %}

In our specific case, we can have at most as many values in the set as there are symbols in &Sigma;'. Hence `int size` should be equal to \|&Sigma;'\|. Remember that `x` and `y` are of type _&#8469;&#8640;&Gamma;\*_ which encodes _&Sigma;'&#8640;&Gamma;\*_.
The type of `concatProd` can be seen as _(&Sigma;'&#8640;&Gamma;\*)&times;(&Sigma;'&#8640;&Gamma;\*) &rarr; &Sigma;'&Sigma;'&#8640;&Gamma;\*_. 

Next we need several utility functions for performing union operations such as singleton union in

> A(x+y) = A(x) &cup; A(y)  

{% highlight C %}
char* unionSingleton(char * lhs,char * rhs){
    if(lhs){
        if(rhs){
            printf("Nondeterminism!");
            exit(1);
        }else{
            return lhs;
        }
    }else{
        return rhs;    
    }
}
{% endhighlight %}

or arbitrarily-sized union like in

> E(x+y) = E(x)&cup;E(y)    

{% highlight C %}
char** unionInPlaceLhs(char ** lhs,char ** rhs,int size){
    for(int i=0;i<size;i++){
        lhs[i]=unionSingleton(lhs[i],rhs[i]);
    }
    return lhs;
}
{% endhighlight %}

or lastly, the union of _&Sigma;'&Sigma;'&#8640;&Gamma;\*_ sets, like in

> L(x+y) = L(x)&cup;L(y)    

{% highlight C %}
char*** union2DInPlaceLhs(char *** lhs,char *** rhs,int size){
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            lhs[i][j]=unionSingleton(lhs[i][j],rhs[i][j]);
        }
    }
    return lhs;
}
{% endhighlight %}

Finally, we need constructors for empty and singleton sets like in

> f(x &isin; &Sigma;') = (&empty;,{(x,&epsilon;)},{(x,&epsilon;)},&empty;)

{% highlight C %}
char * epsilon = "";
char ** empty(int sigmaSize){
    char ** n = malloc(sizeof(char*)*sigmaSize);
    for(int i=0;i<sigmaSize;i++){
        n[i] = NULL;
    }
    return n;
}
char ** singleton(char inputSymbol,char * outputString, int sigmaSize){
    char ** n = empty(sigmaSize);
    n[inputSymbol] = outputString;
    return n;
}
char*** empty2D(int size){
    char *** n = malloc(sizeof(char**)*size);
    for(int i = 0;i<size;i++){
        n[i] = malloc(sizeof(char*)*size);
        for(int j = 0;j<size;j++){
            n[i][j] = NULL;
        }
    }
    return n;
}
{% endhighlight %}

With all those utilities, implementation _f_ will be much easier:

{% highlight C %}
struct T f(struct AST * root, int sigmaSize){
    switch(root->type){
    case 0:{
        struct T x = f(root->uni.lhs,sigmaSize);
        struct T y = f(root->uni.rhs,sigmaSize);
        x.l = union2DInPlaceLhs(x.l,y.l,sigmaSize);
        x.e = unionInPlaceLhs(x.e,y.e,sigmaSize);
        x.b = unionInPlaceLhs(x.b,y.b,sigmaSize);
        x.a = unionSingleton(x.a,y.a);
        return x;
    }
    case 1:{
        struct T x = f(root->concat.lhs,sigmaSize);
        struct T y = f(root->concat.rhs,sigmaSize);
        x.l = union2DInPlaceLhs(concatProd(x.e,y.b,sigmaSize),union2DInPlaceLhs(x.l,y.l,sigmaSize),sigmaSize);
        x.b = unionInPlaceLhs(strConcatSet(x.a,sigmaSize,y.b),x.b,sigmaSize);
        x.e = unionInPlaceLhs(setConcatStr(x.e,sigmaSize,y.a),y.e,sigmaSize);
        x.a = concat(x.a,y.a);
        return x;
    }
    case 2:{
        struct T x = f(root->kleene.child,sigmaSize);
        if(x.a && x.a[0]!='\0'){
            printf("Nondeterminism!");
            exit(1);
        }
        x.l = union2DInPlaceLhs(concatProd(x.e,x.b,sigmaSize),x.l,sigmaSize);
        return x;
    }
    case 3:{
        struct T x = f(root->output.child,sigmaSize);
        x.e = setConcatStr(x.e,sigmaSize,root->output.outStr);
        x.a = concat(x.a,root->output.outStr);
        return x;
    }
    case 4:{
        struct T x;
        x.l = empty2D(sigmaSize);
        x.b = singleton(root->letter.literal,epsilon,sigmaSize);
        x.e = singleton(root->letter.literal,epsilon,sigmaSize);
        x.a = NULL;
        return x;
    }
    case 5:{
        struct T x;
        x.l = empty2D(sigmaSize);
        x.b = empty(sigmaSize);
        x.e = empty(sigmaSize);
        x.a = epsilon;
        return x;
    }
    }
}
{% endhighlight %}

#### Test of progress so far

At this point we might test our code by adding a simple debug function

{% highlight C %}
void printT(struct T * t, int size){
    
    for(int i=0;i<size;i++){
        printf("l:");
        for(int j=0;j<size;j++){
            printf("'%s' ",t->l[i][j]);
        }
        printf("\n");
    }
    printf("b:");
    for(int i=0;i<size;i++){
        printf("'%s' ",t->b[i]);
    }
    printf("\n");
    printf("e:");
    for(int i=0;i<size;i++){
        printf("'%s' ",t->e[i]);
    }
    printf("\n");
    printf("a:'%s'\n",t->a);
}
{% endhighlight %}

We could then add it to our previous `inductivePrint` function.

{% highlight C %}
void inductivePrint(struct AST * root,int size,int indentation){
    struct T t = f(root,size);
    printT(&t,size);
    printSpaces(indentation);
    switch(root->type){
    case 0:
    ...
{% endhighlight %}

Add it to Bison code as follows

{% highlight C %}
Union { 
        int sigmaSize = count($1);
        char * stack = malloc(sizeof(char)* sigmaSize);
        localize($1,0,stack);
        inductivePrint($1,sigmaSize,0);
    }
{% endhighlight %}

You should be able to see the following output:

{% highlight Bash %}
$ echo '"re":"ter"' | ./run.sh 

l:'(null)' '' 
l:'(null)' '(null)' 
b:'' '(null)' 
e:'(null)' 'ter' 
a:'(null)'
:ter
l:'(null)' '' 
l:'(null)' '(null)' 
b:'' '(null)' 
e:'(null)' '' 
a:'(null)'
    .
l:'(null)' '(null)' 
l:'(null)' '(null)' 
b:'' '(null)' 
e:'' '(null)' 
a:'(null)'
        .
l:'(null)' '(null)' 
l:'(null)' '(null)' 
b:'(null)' '(null)' 
e:'(null)' '(null)' 
a:''
            epsilon
l:'(null)' '(null)' 
l:'(null)' '(null)' 
b:'' '(null)' 
e:'' '(null)' 
a:'(null)'
             0
l:'(null)' '(null)' 
l:'(null)' '(null)' 
b:'(null)' '' 
e:'(null)' '' 
a:'(null)'
         1
{% endhighlight %}

### Data structure for automata

We should use hybrid Mealy-Moore machine. Check out the section on more [formal perspective](/transducers_intuitively.html#formal-perspective) for this model. In C we could implement it like this

{% highlight C %}
struct Transition{
    char input;
    int targetState;
    char* output;
};
struct Transitions{
    struct Transition * ts;
    int len;
};
/**The main data structure for entire automaton*/
struct M{ 
    int i; // initial state
    int stateCount;
    struct Transitions * delta; // transition functiom
    char** F; // accepting states and Moore output
};
{% endhighlight %}

We can take `T` produced by `f` function and convert it to `M` as follows.

{% highlight C %}
struct M TtoM(struct T * t,char * stack, int sigmaSize){
    struct M m;
    m.stateCount = sigmaSize+1;//there is one state for every symbols in 
    //Sigma plus one extra initial state 
    m.i = sigmaSize;
    m.F = malloc(sizeof(char*)*m.stateCount);
    for(int i=0;i<sigmaSize;i++)m.F[i] = t->e[i];
    m.F[sigmaSize] = t->a;
    m.delta = malloc(sizeof(struct Transitions)*m.stateCount);
    // delta stores an array of transitions for each state
    for(int i=0;i<sigmaSize;i++){
        int transitionCount = 0;//first find out how many outgoing
        //transitions originate in state i
        for(int j=0;j<sigmaSize;j++)if(t->l[i][j])transitionCount++;
        //then allocate array of transitions
        m.delta[i].len = transitionCount;
        struct Transition * tr = m.delta[i].ts = malloc(sizeof(struct Transition)*transitionCount);
        //then collect all the transitions 
        for(int j=0,k=0;j<sigmaSize;j++){
            char* output = t->l[i][j];
            if(output){
                tr[k].targetState = j; //j is the target state of transition
                tr[k].input = stack[j]; // stack[j] tells us the input label of transition
                tr[k].output = output;
                k++;
            }
        }
    }

    //now it's time to connect initial state with the rest of states
    int initialTransitionCount = 0;

    for(int j=0;j<sigmaSize;j++)if(t->b[j])initialTransitionCount++;
    
    m.delta[sigmaSize].len = initialTransitionCount;
    struct Transition * initialTr = m.delta[sigmaSize].ts = 
        malloc(sizeof(struct Transition)*initialTransitionCount);

    for(int j=0,k=0;j<sigmaSize;j++){
        char* output = t->b[j];
        if(output){
            initialTr[k].targetState = j; 
            initialTr[k].input = stack[j];
            initialTr[k].output = output;
            k++;
        }
    }
    //lastly you could sort all transitions by input in increasing order,
    //which would later allow you to implement binary search. I want to
    //keep things simple so I won't do it here.
    return m;
}
{% endhighlight %}


Let's extend our Bison function and print the produced automaton

{% highlight Bison %}
Union { 
        int sigmaSize = count($1);
        char * stack = malloc(sizeof(char)* sigmaSize);
        localize($1,0,stack);
        struct T t = f($1,sigmaSize);
        struct M m = TtoM(&t,stack,sigmaSize);
        printf("Initial state=%d\n",m.i);
        printf("State count=%d\n",m.stateCount);
        printf("Moore output=");
        for(int i=0;i<m.stateCount;i++){
            printf(" '%s'",m.F[i]);
        }
        printf("\n");
        printf("Transitions:\n");
        for(int i=0;i<m.stateCount;i++){
            for(int j=0;j<m.delta[i].len;j++){
                printf("From %d to %d over '%c' printing \"%s\"\n",i,m.delta[i].ts[j].targetState,m.delta[i].ts[j].input,m.delta[i].ts[j].output);
            }
        }
    }
{% endhighlight %}

You should now see the following output

{% highlight Bash %}
echo '"rete":"ter"+"re":"rtre"' | ./run.sh 

Initial state=6
State count=7
Moore output= '(null)' '(null)' '(null)' 'ter' '(null)' 'rtre' '(null)'
Transitions:
From 0 to 1 over 'e' printing ""
From 1 to 2 over 't' printing ""
From 2 to 3 over 'e' printing ""
From 4 to 5 over 'e' printing ""
From 6 to 0 over 'r' printing ""
From 6 to 4 over 'r' printing ""
{% endhighlight %}

### Running the automata

For execution of automata, we should implement backtracking mechanism

{% highlight C %}
char * run(struct M * m, char * input){
    int len = strlen(input);
    //we will use backtracking mechanism for
    //evaluation of all nondeterministic
    //superpositions of automaton
    int backtrack[len+1][m->stateCount];
    for(int state=0;state<m->stateCount;state++)
        for(int step=0;step<len+1;step++)
            backtrack[step][state]=-1;
    backtrack[0][m->i]=m->i;
    //first we need to propagate input forwards for each symbol 
    for(int step=1;step<=len;step++){
        char inputSymbol = input[step-1];
        for(int state=0;state<m->stateCount;state++){
            if(backtrack[step-1][state]>-1){
                for(int tran=0;tran<m->delta[state].len;tran++){
                    // remember that there might me multiple nondeterministic
                    // transitions for given state and inputSymbol
                    if(m->delta[state].ts[tran].input==inputSymbol){
                        int targetState = m->delta[state].ts[tran].targetState;
                        if(backtrack[step][targetState]>-1){
                            printf("Nondeterminism at step %d in state %d",step,targetState);
                            exit(1);
                        }
                        backtrack[step][targetState] = state; // this will allow us to
                        //backtrack later and determine printed output
                    }
                }
            }
        }
    }
    //now we need to check if any of the accepting states is reached
    int acceptedState = -1;
    for(int state=0;state<m->stateCount;state++){
        if(backtrack[len][state]>-1){
            if(acceptedState==-1){
                acceptedState = state;
            }else{
                printf("Nondeterminism at final step in states %d and %d",acceptedState,state);
                exit(1);
            }
        }
    }
    if(acceptedState==-1){
        //no state accepted, so we return NULL (empty set) as output
        return NULL;
    }
    //now we need to backtrack and collect output printed along each transition
    int sizeOfOutput = strlen(m->F[acceptedState]);
    int backtrackedState = acceptedState;
    for(int step = len;step>0;step--){
        int sourceState = backtrack[step][backtrackedState];
        sizeOfOutput += strlen(outputFor(m,sourceState,backtrackedState,input[step-1]));
        backtrackedState = sourceState;
    }
    char * output = malloc(sizeof(char)*sizeOfOutput+1);
    output[0]='\0';
    strcat(output,m->F[acceptedState]);
    backtrackedState = acceptedState;
    for(int step = len;step>0;step--){
        int sourceState = backtrack[step][backtrackedState];
        char * stepOutput = outputFor(m,sourceState,backtrackedState,input[step-1]);
        int stepOutputLen = strlen(stepOutput);
        memmove(output + stepOutputLen, output, strlen(output) + 1);
        memcpy(output,stepOutput,stepOutputLen);
        backtrackedState = sourceState;
    }
    return output;
}
{% endhighlight %}


We could then extend Bison function to

{% highlight Bison %}
Union { 
        int sigmaSize = count($1);
        char * stack = malloc(sizeof(char)* sigmaSize);
        localize($1,0,stack);
        struct T t = f($1,sigmaSize);
        struct M m = TtoM(&t,stack,sigmaSize);
        printf("Output for 'foo' is '%s'\n",run(&m,"foo"));
    }
{% endhighlight %}

You should then see the following output

{% highlight Bash %}
echo '"foo":"ter"' | ./run.sh 

Output for 'foo' is 'ter'
{% endhighlight %}


At this point the compiler is ready. I purposely ommited many technical details to make the code simpler. I didn't free allocated memory and there is still plenty of room for extra optimisations. However, those problems are of secondary importance. The key point of this tutorial was just showing how the algorithm and compiler should work in principle.

You can see the full code here:

[https://github.com/aleksander-mendoza/aleksander-mendoza.github.io/tree/master/code/mealy_compiler](https://github.com/aleksander-mendoza/aleksander-mendoza.github.io/tree/master/code/mealy_compiler)

BONUS: Here you can see the code without memory leaks: 
[https://github.com/aleksander-mendoza/aleksander-mendoza.github.io/tree/master/code/mealy\_compiler\_web\_and\_no\_leaks](https://github.com/aleksander-mendoza/aleksander-mendoza.github.io/tree/master/code/mealy_compiler_web_and_no_leaks) 
It also shows how to compile it with Emscripten to WebAssembly. Here you can see an [online example](/code/mealy_compiler_web_and_no_leaks/bin/web_mealy_compiler.html).
