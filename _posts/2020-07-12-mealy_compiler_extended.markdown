---
layout: post
title:  "Compiler of Mealy automata (extended final version)"
date:   2020-07-12 01:07:00 +0200
categories: automata
permalink: /mealy_compiler_extended.html
---
**Contents**
* TOC
{:toc}

This post is the final extension of [previous mealy machine compiler](/mealy_compiler.html). Here I show how to add weights, variables and ranges.

### Syntax grammar

First we need to chaneg the syntax. In the previous post the grammar was


    Union ::= Concat '+' Union | Concat
    Concat ::= Kleene Concat | Kleene
    Kleene ::= Product '*' | Product
    Product ::= Atomic ':' QUOTED_STRING | Atomic
    Atomic ::= QUOTED_STRING | '(' Union ')'
    
    
Now it should be as follows:

    Union ::= WEIGHT? Concat '+' Union | WEIGHT? Concat
    Concat ::= Kleene WEIGHT? Concat | Kleene WEIGHT?
    Kleene ::= Product WEIGHT? '*' | Product
    Product ::= Atomic ':' QUOTED_STRING | Atomic
    Atomic ::= QUOTED_STRING | RANGE | ID | '(' Union ')'
    
where  

- `WEIGHT?` is an optional token that matches any integer literal (like for instance `WEIGHT ::= -?[0-9]+`),  
- `RANGE` is a Unix-style range (like `RANGE ::= \[\\?.-\\?.\]`, so for instance any of `[a-b]`, `[0-9]`, `[\n-\\]`, `[ - ]`, but not `[ab-e]`, `[-]`, `[ e-e]`).
- `ID` is an identifier (like `ID ::= [a-zA-Z_][a-z0-9A-Z_]*`, for instance `var1` or `_`) 

Such grammar allows us to insert weights at any point. For instance all the following are valid weighted regexes:

    "foo" 1 
    1 "foo"
    1 "foo" 1
    1 "foo" 1 + -1 "bar" 2 
    "baz" 3 *

You can notice that `Union` matches all those weights in front of expressions and `Concat` matches those at the end. Addionally, you can also put weight under Kleene closure. You can't write multiple weights next to each other like `"foo" 1 2 "bar"` (it would be pointless to do so anyway), however, you can always resort to `("foo" 1) 2 "bar"`. 

Those weights will later be translated into weighted transitions in automaton, like [this one](/transducers_intuitively.html#weighted-automata), except that in our case, only integer-valued weigths are allowed (no fractions/decimal points). This is for a reason, which I explain [here](/weighted_transducers.html) in detail. 

### Abstract syntax tree (second stage)

Previously our AST looks as follows

> Every letter is a regex  
> &epsilon; is a regex  
> if x and y are regexes then union x+y is a regex  
> if x and y are regexes then concatenation  xy is a regex  
> if x is a regex then Kleene closure x* is a regex  
> if x is a regex and y is a string then x:y is a regex  

which in more Haskell-like notation can be written as

    AST = Union AST AST | Concat AST AST | Kleene AST 
         | Output AST String | Letter Char | Epsilon

Now, however, we need to extend it as follows:


> Every letter is a regex  
> &epsilon; is a regex  
> if x and y are regexes then union x+y is a regex  
> if x and y are regexes then concatenation  xy is a regex  
> if x is a regex then Kleene closure x* is a regex  
> if x is a regex and y is a string then x:y is a regex  
> if x is a regex and y is a weight then right-side weight addition x+y is a regex  
> if x is a weight and y is a regex then left-side weight addition x+y is a regex  


alternatively written as

    AST = Union AST AST | Concat AST AST | Kleene AST 
         | WeightBefore Int AST | WeightAfter AST Int
         | Output AST String | Range Char Char | Epsilon

Instead `Letter` we only have `Range` and every letter `x` will now be translated to range `[x-x]`. Moreover, we also added weights, but we need to keep track of the exact position of each weight. This will be crutial for Glushkov's algorithm. Hence we have two separate nodes `WeightBefore` and `WeightAfter`, which will later correspond to adding weights from left and right side. Notice that in our grammar

    Union ::= WEIGHT? Concat '+' Union | WEIGHT? Concat
    Concat ::= Kleene WEIGHT? Concat | Kleene WEIGHT?
    Kleene ::= Product WEIGHT? '*' | Product
    ...
    
the `Union` rule corresponds to generating `WeightBefore` and `Concat` to `WeightAfter`. `Kleene` also generates `WeightAfter`, but the weight is "under" closure. 

### Abstract syntax tree (first stage)

Notice how in the AST above, there was no room for variables, even though we clearly had `ID` token in our grammar. That's because now we actually need to have two separate kinds of abstract syntax tree. There will be two stages of AST generation. In the first stage we allow for variables, but in the second stage we substitute all of them. For example consider this regular expression:

    var1 = "foo" : "bar"
    var2 = var1* "baz"

Compiler should generate two separate (first stage) AST for each variable and obtain


    HashMap<String,ASTFirstStage> varDefinitions = {
      "var1" : Output(
        Concat(
          Range('f','f'),
          Concat(
            Range('o','o'),
            Range('o','o')
          )
        ),
        "bar"
      ),
      "var2" : Concat(
         Kleene(
           Var("var1")
         ),
         Concat(
           Range('b','b'),
           Concat(
             Range('a','a'),
             Range('z','z')
           )
         )
       )
    }

Then in the following step, the compiler must substitute every occurence of `Var(...)` with the corresponding entry from hashmap. We will later implement a function like this


    HashMap<String,ASTSecondStage> substituteVars(ASTFirstStage,HashMap<String,ASTSecondStage>)

which we should use to build `HashMap<String,ASTSecondStage>` more or less like this:

{% highlight Java %}
HashMap<String,ASTSecondStage> substituteAll(HashMap<String,ASTFirstStage> varDefinitions){
    HashMap<String,ASTSecondStage> newVars = {}
    //It's very important to iterate in the same order
    //as the definition order in original source code.
    //Some languages implement LinkedHashMap for that purpose.
    for varName, varAST in varDefinitions{
        newVars[varName] = substituteVars(varAST, newVars)
    }
    return newVars
}
{% endhighlight %}

##### Circular dependencies

The order of iteration in the loop above is crucial. We need to detect two kinds of fatal errors: circular dependency and undefined varaibles. For instance

    var1 = "foo" var0  // oh no! undefined var0 is undefined
    var2 = "bar" var3
    var3 = "baz" var2 // oh no! Circular dependency gets expanded infinitely
    
There are two ways to tackle this problem. 

1\. Don't allow forward references. Only previously defined variables are allowed to be used at a given stage. So this is not allowed

    var1 = "foo" var2
    var2 = "bar"

instead you have to reorder variables as follows

    var2 = "bar"
    var1 = "foo" var2
    
This way introduction of circular dependencies is impossible. Notice that every code that doesn't have circular dependencies, can be reordered in such a way that no forward-reference occurs (so we don't lose any expressive power).

2\. Use Arden's theorem to convert infinities into Kleene closures. For instance this

    var1 = "foo" var2
    var2 = "bar" var1

would result in 

    var1 = "foo" "bar" "foo" "bar" ...
    var2 = "bar" "foo" "bar" "foo" ...

which in fact is equivalent to

    var1 = ("foo" "bar")*
    var2 = ("bar" "foo")*


Unfortuantely approach number 2 is severely limited in the number of cases that it can handle. For instance 

    var1 = "a" var1 "b"
    
is impossible to compile, because it isn't even regular language at all (it's context-free and requires pushdown automata). Similalry, Adren's theorem fails on expressions with Kleene closure, like the following

    var1 = "foo" var2
    var2 = "bar" var1*


Moreover, notice that there is no difference at all, when it comes to expressive power. Everything that can be solved with Arden's theorem, can also be expressed without it using the first approach. 

Therefore, in the rest of this tutorial, we will go with approach number 1.

##### AST (first stage) definition

To wrap this section up, I finally present you the definition of first stage AST


    ASTVar = Union ASTVar ASTVar | Concat ASTVar ASTVar | Kleene ASTVar 
         | WeightBefore Int ASTVar | WeightAfter ASTVar Int
         | Output ASTVar String | Range Char Char | Epsilon
         | Var String

The only difference between `AST` and `ASTVar ` is the `Var String` node.


### Glushkov construction with weights

Recall that Glushkov's algorithm is meant to work with localized languages. The main idea behind it is to treat every symbol of regular expression as separate state of automaton and check which symbols can come after the other. For instance consider the very simple expression

    ab
 
 where a&isin;&Sigma; and b&isin;&Sigma;. You can reach _b_ right after _a_. Therefore in the produced automaton there will be a transition from _a_ to _b_.
 
 ![fsa_eps_a_b](/assets/fsa_eps_a_b.png) 

Now consider the weighted regular expression 

    a 1 b
    
You can reach _b_ after _a_ just like before, but this time, you would need to cross _1_ on your way. Therefore the corresponding weighted automaton will look like this:

 ![fsa_eps_a_b](/assets/fsa_eps_a_b_weighted.png) 
 
In more complex cases you might need to deal with unions like this

    (a + b + c)  (d + e + f)
    
![fsa_eps_abc_def](/assets/fsa_eps_abc_def.png)

The weighted counterpart might look like this

    (a 1 + b 2 + c 3) 100 ( 10 d + 20 e + 30 f)
    
Here the transition between _a_ and _d_ has weight _1+100+10=111_, transition between _a_ and _e_ has _1+100+20=121_ and so on. The only problem is that you cannot change weight between _a_ and _c_, without accidentaly changing some other weights. The solution would be to split the expression like this

    (a 1 + b 2 + c 3) 100 ( 10 d + 20 e + 30 f) 
    =
    (a 1 + b 2) 100 ( 10 d + 20 e + 30 f) +
    (c 3) 100 ( 10 d + 20 e + 30 f) 
    =
    (a 1) 100 ( 10 d + 20 e + 30 f) +
    (b 2) 100 ( 10 d + 20 e + 30 f) +
    (c 3) 100 ( 10 d + 20 e + 30 f) 
    
Now you have enough degrees of freedom to preciesly adjust every single transition to you liking. For instance

    (a 1) 100 ( 10 d + 20 e + 30 f) +
    (b 2) 100 ( 40 d + 50 e + 60 f) +
    (c 3) 100 ( 70 d + 80 e + 90 f) 
    
Unfortunately, this solution is a tradeoff between precision and number of state. Now instead of 7 states, Glushkov's construction will generate 13. This problem can be countered by implementing minimisation algorithm. 

##### Disgression on minimisation algorithm

Recall that the automata generated by us are nondeterministic and &epsilon;-free. The problem of minimisation for nondeterministic machines is very difficult (probably it's P-SPACE complete but it's only a conjecture). Moreover, our automata are weighted which makes things even more difficult. Here [here](/weighted_transducers.html) we prove that weighted automata can be exponentially smaller than even the smallest non-weighted mealy machines. Hence, if we want to minimise them, we shouldn't convert them to non-weighted and then use non-weighted minimisation, because most likely we will end up worse-off than we started. This means that the only serious candidate for minimisation is the [approach proposed by Tiko Kameda and Peter Weiner](https://www.researchgate.net/publication/3045459_On_the_State_Minimization_of_Nondeterministic_Finite_Automata). However, in order to deal with such simple cases like described in examples above, we don't actually need a full minimisation algorithm. There exists a heuristic pseudo-minimisation algorithm, that works specifically for nondeterministic mealy machines and very often finds "smaller" automata, even though they don't necessarily need to be the smallest. The idea behind this algorithm is as follows.

Let _&delta;:Q&times;&Sigma;&rarr;2<sup>Q&times;&Gamma;*</sup>_ be the transition function.  
Find two states _q<sub>1</sub>,q<sub>2</sub>&isin;Q_ such that for all _x&isin;&Sigma;_ their transitions are equivalent _&delta;(q<sub>1</sub>,x)=&delta;(q<sub>2</sub>,x)_. Merge those two states into one (therefore the total number of states decreases by one). Repeat until no more pair of such states could be found.  
Additionally, to speed up this algorithm, the _&delta;_ function could be represented by an array, whose elements are sorted in such a way, than similar states would be next to each other (for instance, if each element of array stores transitions for some state _q_, then the array could be sorted according to the sum of _|&delta;(q,x)|_ for all _x_).

##### Epsilons

There is one case that requires special consideration. Consider

    "a" 1 "" 2 "b"
    
What should be the weight of transition between _a_ and _b_? 

##### Inductive definition

Now that I explained the idea, we can move on to actual Glushkov's algorithm.


# WORK IN PROGRESS

