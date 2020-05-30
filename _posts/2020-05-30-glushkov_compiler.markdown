---
layout: post
title:  "Glushkov construction in C"
date:   2020-05-30 01:08:00 +0200
categories: automata
permalink: /glushkov_compiler.html
---
**Contents**
* TOC
{:toc}
  
Note: This post is based on [quick compiler tutorial](/compiler_in_c.html) written earlier. If you don't have any experience with riting compilers, check the other post first. Knowledge of [Glushkov's construction](/compiling_regexes.html#glushkovs-construction) is also necessary. 


### Goal of the tutorial

This tutorial can teach you two things. One of them is of course in depth understanding of Glushkov's construction. The second one is showing you example of a more complex compiler, but at the same time simple enough to cover it in one post. It's a good exercise before exploring compilers for Turing-complete languages (Glushkov's construction compiles finite state automata, which are not Turing-complete)

I skip the setup part of the tutorial, because it's the same as in [the previosu post](/compiler_in_c.html).

### Grammar

First of all, we need to define our grammar. We are going to compile regular expressions, hence the inductive definition should look like this:

> 1. Every letter is a regular expression
> 2. If x and y are regular expressions, then xy is a regular expression (concatenation)
> 3. If x and y are regular expressions, then x+y is a regular expression (union)
> 4. If x is a regular expressions, then x* is a regular expression (Kleene closure)

Kleene closure binds the strongest, then concatenation and union is the weakest. For simplicity I will assume that the only allowed letters are `a-zA-Z0-9` and space. We should also use brackets when necessary. This gives rise to the following grammar:

> Union ::= Union '+' Concat \| Concat  
> Concat ::= Kleene  Concat \| Kleene  
> Kleene ::= Letter '*' \| Letter  
> Letter ::= [a-zA-Z0-9] \| '(' Union ')'  

### Basic parser with AST

This part is mostly analogical to the previous tutorial.
We should create the following lexer `syntax_scanner.l`:

{% highlight flex %}
%{ 
#include "syntax_parser.h" 
%}
letter          [a-zA-Z 0-9]

%option noyywrap  
%%

"+"	    return(PLUS); 
"("	    return(BEG);
")"	    return(END);
"*"	    return(STAR);

{letter}	{ 
       yylval.cval = *yytext;
       return(LETTER);
}

{% endhighlight %}

and the following grammar `syntax_parser.y`:

{% highlight bison %}
%{

#include <stdio.h>
#include "main.h"

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
        inductivePrint(root->uni.lhs,indentation+4);
        inductivePrint(root->uni.rhs,indentation+4);
        return;
    case 1:
        printf(".\n");
        inductivePrint(root->concat.lhs,indentation+4);
        inductivePrint(root->concat.rhs,indentation+4);
        return;
    case 2:
        printf("*\n");
        inductivePrint(root->kleene.child,indentation+4);
        return;
    case 3:
        printf("%c\n",root->letter.literal);
        return;
    }
}

%}

%union
{
    char cval; 
    struct AST * ast;
};

%token <cval> LETTER
%token BEG
%token END
%token PLUS
%token STAR

%type<ast> Union
%type<ast> Concat
%type<ast> Kleene
%type<ast> Letter

%start Start

%%
Start:
   Union { inductivePrint($1,0); }
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
    Letter {$$ = $1;}
    | Letter STAR { $$ = mkKleene($1); }
    ;

Letter:
    LETTER { $$ = mkLetter($1); }
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

The necessary definitions of data structures are specified in `main.c`

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
struct AST * mkLetter(char c){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 3;
	struct ASTLetter letter = {c};
	node->letter = letter;
	return node;
}
{% endhighlight %}
 

Lastly there is with `main.h`

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


struct AST{
	/**0 for union;1 for concatenation; 
	2 for kleene closure; 3 for letter literal*/
	int type;
	union{
		struct ASTUnion uni;
		struct ASTConcat concat;
		struct ASTKleene kleene;
		struct ASTLetter letter;
	};
};

struct AST * mkUnion(struct AST * lhs,struct AST * rhs);
struct AST * mkConcat(struct AST * lhs,struct AST * rhs);
struct AST * mkKleene(struct AST * child);
struct AST * mkLetter(char literal);

#endif

{% endhighlight %}

You can now run `./compile.sh` and then you should see:

    $ echo "1+fe(r)*" | ./run.sh 
    +
      1
      .
        f
        .
          e
          *
            r


### Implementing Glushkov's algorithm

In the post about [compiling regexes](/compiling_regexes.html#glushkovs-construction) I showed an intuitive example of how Glushkov's construction should work. However, now that we want to implement it, we need to be more technical and specific. Therefore we need to first define several inductive procedures. First, let's count the number of letters:

> count(x+y) = count(x)+count(y)  
> count(xy) = count(x)+count(y)  
> count(x*) = count(x)  
> count([a-zA-Z0-9 ]) = 1

In C this becomes:


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
        return 1;
    }
}

{% endhighlight %}

Next we need to perform localization of language. We will replace every literal characetr with some new unique number and also store somewhere an array that would tell us which number corrsponds to which letter. Unfortunately, this operation by it's very nature requires mutation of variables. This never looks pretty in mathematics and requires monadic magic. It's much easier therefore to give you the C code directly:

{% highlight C %}
/**We should push consecutive renamed 
 letters onto stack. We can know how much to allocate
 ahead of time thanks to count() function. This 
 localize() function returns the total height of 
 used stack (we never actually pop 
 from the stack).*/
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
        stack[offset] = root->letter.literal;
        root->letter.literal = (char)offset;
        return offset+1;
    }
}
{% endhighlight %}

If you want to test how it works, you can use the following print function:

{% highlight C %}
printSpaces(int indentation){
    for(int i=0;i<indentation;i++){
        printf(" ");
    }
}
void inductivePrint(struct AST * root, char* arr,int indentation){
    printSpaces(indentation);
    switch(root->type){
    case 0:
        printf("+\n");
        inductivePrint(root->uni.lhs,arr,indentation+2);
        inductivePrint(root->uni.rhs,arr,indentation+2);
        return;
    case 1:
        printf(".\n");
        inductivePrint(root->concat.lhs,arr,indentation+2);
        inductivePrint(root->concat.rhs,arr,indentation+2);
        return;
    case 2:
        printf("*\n");
        inductivePrint(root->kleene.child,arr,indentation+2);
        return;
    case 3:
        printf("%c %d\n",arr[root->letter.literal], root->letter.literal);
        return;
    }
}
{% endhighlight %}

and add this to grammar:

{% highlight bison %}
Start:
    Union { 
        int stack_size = count($1);
        char * stack = malloc(sizeof(char)* stack_size);
        localize($1,0,stack);
        inductivePrint($1,stack,0);
    }
    | 
    ;
{% endhighlight %}

Then run `./compile.sh` and you should see

{% highlight bash %}
$echo "1+fe(r)*" | ./run.sh 
+
  1 0
  .
    f 1
    .
      e 2
      *
        r 3
{% endhighlight %}

So far so good. We have working localization. Now is the time for the main part. Define 3 functions: `A`, `B`, `E` and `L`. `B` will take AST and return all the (localized) letters which can be found at the beginning of any string accepted by the regular expression. Similarly `E` returns all ending letters. `L` returns all the substrings of size 2 which can be found in any accepted string. `A` is responsible only for checking whether given expression accepts empty string or not (returns true or false). Their types could be summarized as _A:AST&rarr;{1,0}_, _B:AST&rarr;2<sup>&Sigma;</sup>_, _E:AST&rarr;2<sup>&Sigma;</sup>_ and _L:AST&rarr;2<sup>&Sigma;&times;&Sigma;</sup>_ where _&Sigma;_ is the alphabet after localization. Below are their inductive definitions:

> A(x+y) = A(x) or A(y)  
> A(xy) = A(x) and A(y)
> A(x*) = 1  
> A([a-zA-Z0-9 ]) = 0  


> B(x+y) = B(x)&cup;B(y)   
> B(xy) = B(x)&cup;B(y) if A(x) else B(x)   
> B(x*) = B(x)  
> B(x) = {x} where x&isin;[a-zA-Z0-9 ]


> E(x+y) = E(x)&cup;E(y)   
> E(xy) = E(x)&cup;E(y) if A(y) else E(y)   
> E(x*) = E(x)  
> E(x) = {x} where x&isin;[a-zA-Z0-9 ]


> L(x+y) = L(x)&cup;L(y)   
> L(xy) = E(x)&times;B(y) &cup; L(x) &cup; L(y)   
> L(x*) = E(x)&times;B(x) &cup; L(x)  
> L(x) = &empty; where x&isin;[a-zA-Z0-9 ]

Theoretically we could implement all of these functions separately and the algorithm would work. It's actually the easy and naive way of doing it. There is, however, a much more efficient approach, but it's also a bit more complicated. We could merge all of those inductive prodecures into one `X` of the joined type _X:AST &rarr; 2<sup>&Sigma;&times;&Sigma;</sup> &times; 2<sup>&Sigma;</sup> &times; 2<sup>&Sigma;</sup> &times; {0,1}_.

> X(x+y) = (l<sub>x</sub> &cup; l<sub>y</sub>,b<sub>x</sub> &cup; b<sub>y</sub>,e<sub>x</sub> &cup; e<sub>y</sub>,a<sub>x</sub> or a<sub>y</sub>)   
> where X(x)=(l<sub>x</sub>, b<sub>x</sub>, e<sub>x</sub>, a<sub>x</sub>), X(y)=(l<sub>y</sub>, b<sub>y</sub>, e<sub>y</sub>, a<sub>y</sub>)  

> X(xy) = (e<sub>x</sub> &times; b<sub>y</sub> &cup; l<sub>x</sub> &cup; l<sub>y</sub>,
b<sub>x</sub> &cup; b<sub>y</sub> if a<sub>x</sub> else b<sub>x</sub>,
e<sub>x</sub> &cup; e<sub>y</sub> if a<sub>y</sub> else e<sub>y</sub>,
a<sub>x</sub> and a<sub>y</sub>)  
> where X(x)=(l<sub>x</sub>, b<sub>x</sub>, e<sub>x</sub>, a<sub>x</sub>), X(y)=(l<sub>y</sub>, b<sub>y</sub>, e<sub>y</sub>, a<sub>y</sub>)

> X(x*) = (e<sub>x</sub> &times; b<sub>x</sub> &cup; l<sub>x</sub>,
b<sub>x</sub> ,
e<sub>x</sub>,
1)  
> where X(x)=(l<sub>x</sub>, b<sub>x</sub>, e<sub>x</sub>, a<sub>x</sub>)

> X(x) = (&empty;, {x}, {x}, 0)  where x&isin;[a-zA-Z0-9 ]

If we tried to actually implement it in C, we would find it to be rather challenging. The operations of Cartesian product &times; and set union &cup; look good on paper, but computers require heavy machinery if hash sets, hash maps etc. I will use here a much simpler and easier to understand solution. It might not be the fastest one, but it should be just fine. The idea is to use a matrix _n&times;n_ where _n_ is the number of (localized) letters. Each cell in the matrix would encode one element of _2<sup>&Sigma;&times;&Sigma;</sup>_.


The C procedure then becomes this
{% highlight C %}
typedef unsigned char bool;

bool glushkov(struct AST * root, int n, bool * B, bool * E, bool ** L){
    switch(root->type){
    case 0:
        return glushkov(root->uni.lhs,n,B,E,L) || 
            glushkov(root->uni.rhs,n,B,E,L);
    case 1:{
        bool * Bx = malloc(sizeof(bool)*n);
        bool * Ex = malloc(sizeof(bool)*n);
        bool * By = malloc(sizeof(bool)*n);
        bool * Ey = malloc(sizeof(bool)*n);
        for(int i=0;i<n;i++){
            Bx[i] = 0;
            Ex[i] = 0;
            By[i] = 0;
            Ey[i] = 0;
        }
        bool Ax = glushkov(root->uni.lhs,n,Bx,Ex,L);
        bool Ay = glushkov(root->uni.rhs,n,By,Ey,L);
        if(Ax){
            for(int i=0;i<n;i++){
                B[i] |= Bx[i] | By[i];
            }
        }else{
            for(int i=0;i<n;i++){
                B[i] |= Bx[i];
            }
        }
        if(Ay){
            for(int i=0;i<n;i++){
                E[i] |= Ex[i] | Ey[i];
            }
        }else{
            for(int i=0;i<n;i++){
                E[i] |= Ey[i];
            }
        }
        for(int i=0;i<n;i++){
            if(Ex[i]){
                for(int j=0;j<n;j++){
                    if(By[j]){
                        L[i][j] = 1;
                    }
                }
            }
        }
        free(Bx);
        free(Ex);
        free(By);
        free(Ey);
        return Ax && Ay;
    }
    case 2:{
        bool * Bx = malloc(sizeof(bool)*n);
        bool * Ex = malloc(sizeof(bool)*n);
        for(int i=0;i<n;i++){
            Bx[i] = 0;
            Ex[i] = 0;
        }
        glushkov(root->kleene.child,n,Bx,Ex,L);
        for(int i=0;i<n;i++){
            if(Ex[i]){
                for(int j=0;j<n;j++){
                    if(Bx[j]){
                        L[i][j] = 1;
                    }
                }
            }
        }
        for(int i=0;i<n;i++){
            E[i] |= Ex[i];
        }
        for(int i=0;i<n;i++){
            B[i] |= Bx[i];
        }
        free(Bx);
        free(Ex);
        return 1;
    }
    case 3:
        E[root->letter.literal] = 1;
        B[root->letter.literal] = 1;
        return 0;
    }
}
{% endhighlight %}

It's quite long but this hard to get something simpler than that. 