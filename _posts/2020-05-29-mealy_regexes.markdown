---
layout: post
title:  "Regexes for Mealy machines"
date:   2020-05-28 01:07:00 +0200
categories: automata
permalink: /mealy_regexes.html
---

Every finite state automaton can be represented with a regular expression and vice versa. The same holds true for Mealy machines but the expressions are different. There are two approaches to them: 
- one that uses "standrad" regular expressions and extends them with output
- one that treats everything as input-output pairs from the very beginning.

Note: Throughout this article I use `+` instead of `|` to denote union. People who are used to UNIX regexes but don't have theoretical background might find it confusing. Similarly I use &epsilon; to denote empty string `""`, which doesn't have representation in UNIX.

### Approach 1: extension of regular expressions

> 1. every letter is a regular expression (including empty string &epsilon;) 
> 2. if _x_ and _y_ are regular expressions then _xy_ is a regular expression (concatenation)
> 3. if _x_ and _y_ are regular expressions then _x+y_ is a regular expression (union)
> 4. if _x_ is regular expressions then _x*_ is a regular expression (Kleene closure)
> 5. if _x_ and _y_ are regular expressions then _x:y_ is regular relation (Cartesian product of languages).
> 6. if _x_ and _y_ are regular relations then _xy_ is regular relation (concatenation).
> 7. if _x_ and _y_ are regular relations then _x+y_ is regular relation (union).
> 8. if _x_ is regular relation then _x*_ is regular relation (Kleene closure).

So in essence the idea is to treat regular expressions and regular relations as separate types with distinct operations. For instance

    ca(a+b)*
    a*+c

are two regular expressions, then

    (ca(a+b)*) : (a*+c)
    
is a regular relation. `ca(a+b)*` matches strings such as `ca`, `caa`, `caba`. `a*+c` may match empty string or `a`, `c`, `aaa`. The Cartesian product `:` of both yields a regular relation, which for input `ca` will output `a`, `c`, `aaa` and so on. For input `caa` it will also produce the same output. In general for any input that matches `ca(a+b)*` it will output all (in this case infinitely many) strings produced by `a*+c`. 

Then there are operations of concatenation, union and Kleene closure that work specifically with regular relations. Suppose that `M` and `N` are some regular relations. `M` produces `y` (among other possible outputs) for input `x`. Similarly `N` produces `b` for input `a`. Then

1. concatenation `MN` will produce `yb` (among other outputs) for input `xa`.
2. union `M+N` will produce `y` for input `x` and `b` for input `a`
3. Kleene closure `M*` will produce empty string for input empty string, `y` for input `x`, `yy` for input `xx` and so on.

### Approach 2: everything is relation

> 1. every pair of letters _a:b_ is a regular relation. _a_ and/or _b_ might also be empty strings.
> 2. if x and y are regular relations then xy is a regular relation (concatenation)
> 3. if x and y are regular relations then x+y is a regular relation (union)
> 4. if x is a regular relation then x* is a regular relation

This approach is much simpler than the previous one. The regular relation:

> (c:&epsilon;)(a:&epsilon;)((a:&epsilon;)+(b:&epsilon;))*

is equivalent to `ca(a+b)*` from previous example. Similarly

> (a:&epsilon;)*+(c:&epsilon;)

is equivalent to `a*+c`. In general every regular expression describes some formal language and every formal language is subset of _&Sigma;*_. There is not much difference between _&Sigma;*&times;**1**_ and the _&Sigma;*_ itself (here _**1**_ is a some arbitrary singleton set _**1**={1}_). It even resembles multiplication from arithmetic _x&middot;1=x_ very much.

The only catch is that we no longer can use `:` to perform cartesian product. This is not a real problem, though. We should notice that 

> a:(b+c)

can be obtained by performing

> a:b+a:c

Similarly the following equalities hold:

> (b+c):a = b:a + c:a  
> (bc):a = (b:a)(c:&epsilon;) = (b:&epsilon;)(c:a)  
> a:(bc) = (a:b)(&epsilon;:c) = (&epsilon;:b)(a:c)  
> a\*:b = (a:&epsilon;)\*(&epsilon;:b)  
> a:b\* = (a:&epsilon;)(&epsilon;:b)\*  
> (b+c)\*:&epsilon; = (b:&epsilon;+c:&epsilon;)\*  
> (bc)\*:&epsilon; = (b:&epsilon; c:&epsilon;)\*  

Therefore we can convert `(ca(a+b)*) : (a*+c)` as

(ca(a+b)\*) : (a\*+c) =  
= (ca(a+b)\*) : (a\*\) + (ca(a+b)\*):c =  
= (ca(a+b)\*):&epsilon; (&epsilon;:a)\* + (ca(a+b)\*):c =  
= (ca):&epsilon; (a+b)\*:&epsilon; (&epsilon;:a)\* + (ca(a+b)\*):c =  
= c:&epsilon; a:&epsilon; (a+b)\*:&epsilon; (&epsilon;:a)\* + (ca(a+b)\*):c =  
= c:&epsilon; a:&epsilon; (a:&epsilon;+b:&epsilon;)\* (&epsilon;:a)\* + (ca(a+b)\*):c =  
= c:&epsilon; a:&epsilon; (a:&epsilon;+b:&epsilon;)\* (&epsilon;:a)\* + (ca(a+b)\*):&epsilon; &epsilon;:c =  
= c:&epsilon; a:&epsilon; (a:&epsilon;+b:&epsilon;)\* (&epsilon;:a)\* + c:&epsilon; a:&epsilon; (a:&epsilon;+b:&epsilon;)\* &epsilon;:c 


### Special case of functional Mealy machines

Everything that was presented above described nondeterministic Mealy machines with &epsilon;-transitions (including possibility of &epsilon;-cycles). However, it's not very practical to work with such regular relations, because nobody wants to wait for generation of infinitely many outputs (it would take literally forever). Usually we are more interested only in functional machines. This restricts regular relations a lot. The following constructions are illegal

> a:(b+c)  
> a:(b*)

because they always introduce nondeterministic output. Care should be taken because it's still legal to write _a:b + c:d_ and it's possible that _a_ and _b_ might intersect (like for example `(0+01):1 + 0(1+01):0` would produce both `0` and `1` for input `01`). This leads to alternative definition of regular relations

> 1. every letter _x_ (including &epsilon;) is a regular relation equivalent to x:&epsilon;
> 2. if _x_ and _y_ are regular relations then _xy_ is a regular relation
> 3. if _x_ and _y_ are regular relations then _x+y_ is a regular relation
> 4. if _x_ is a regular relation then _x*_ is a regular relation
> 5. if _x_ is a regular relation and _y_ is some string then _x:y_ is a regular relation equivalent to x(&epsilon;:y)

These regular expressions still have the same power as the two previous definitions. The only difference is that this approach plays really well with Glushkov's construction and is easier to compile. It is also a lot easier to write expressions in this system. Instead of writing:

> a:&epsilon; b:&epsilon; c:&epsilon; &epsilon;:a &epsilon;:b

You can just write

> abc:ab

Moreover, the notation `ab:c*` is automatically understood as `(ab:c)*` instead of `ab:(c*)`, which makes a lot more sense for functional machines.

Thanks to the guarantee of point 5 that _y_ is just a string, inductive reasoning about such expressions becomes often simplified by a great margin (no need to perform deeper induction of the output side of expression). 




### Distinct alphabets and composition

All of the definitions above can easily be generalised to regular relations of different alphabets _&Sigma;\* &times; &Gamma;\*_ instead of _&Sigma;\* &times; &Sigma;\*_. Most of the time it's not necessary to specify it explicitly, because _&Sigma;\* &times; &Gamma;\*_ can be emulated with _(&Sigma; &cup; &Gamma;)\* &times; (&Sigma; &cup; &Gamma;)\*_. There are cases when keeping track of alphabets becomes important, though. This is especially when we want to extend regular relations with composition operation.

Let _M_ and _N_ be a regular relations such that _M &sub; &Sigma;\* &times; &Gamma;\*_ (for _M_ input alphabet is  _&Sigma;_ and output is _&Gamma;_) and _N &sub; &Gamma;\* &times; &Omega;\*_. Then we can compose them and obtain _N &#9675; M &sub; &Sigma;\*&times;&Omega;\*_. If _(x,y) &isin; M_ and _(y,z) &isin; N_ then _(x,z) &isin; N &#9675; M_ . Here is an example:

_&Sigma;={0,1}_  
_&Gamma;={a,b}_  
_&Omega;={x,y}_  
_M = {(00,a), (01,a), (&epsilon;,abb), (00,&epsilon;)}_  
_N = {(a,x), (a,xy), (&epsilon;,xxxx)}_   
_N &#9675; M = {(00,x), (00,xy), (01,x), (01,xy), (00,xxxx)}_   

We can add this operation to regular expressions, however it is not really necessary. All compositions can be converted to equivalent regular expressions without them. However, sometimes the resulting expression might  be exponentially larger. If you are interested, here is a proof (but you can skip it).

#### Exponential pessimistic case for composition

Consider a relation _M = {(&epsilon;,&epsilon;), (xx,x), (xxxx,xx), ...}_ over alphabet _&Sigma;\*&times;&Sigma;\*_. This relation can be described with regular expression _(xx:x)*_. The composition _M &#9675; M_ will yield relation  
 _M &#9675; M = {(&epsilon;,&epsilon;), (xxxx,x), (xxxxxxxx,xx), ...}_  
which can be described with _(xxxx:x)\*_. Analogically  _M &#9675; M &#9675; M_ is equivalent to _(xxxxxxxx:x)\*_. This regular expression is minimal (there is no other expression that would have less input symbols and still be equivalent). Notice that size of _(xx:x)\*_ is 2 (it has two input symbols: _xx_ ). Therefore the size of 
 
_(xx:x)* &#9675; (xx:x)* &#9675; (xx:x)* ... &#9675; (xx:x)*_  

is _2&middot;(n+1)_ where _n_ is the number of compositions _&#9675;_. The size of equivalent regular expression without composition is _2<sup>n+1</sup>_. 

End of proof.

You may also see that in this proof there is the strings of letter _x_ can be thought of as numbers. For instance _0=&epsilon;_, _1=x_, _2=xx_ and so on. Then the expression _(xx:x)*_ is equivalent to division by _2_. We could then write _(xx:x)* = 1/2_. The composition becomes equivalent to multiplication. For instance _(xx:x)* &#9675; (xx:x)* = 1/2 &middot; 1/2 = 1/4_. Therefore performing _n_ such multiplications results in _1/2<sup>n+1</sup>_.

#### Typed regular expressions

One could think of the distincs alphabets as types. We could then annotate every letter with its respective type. For instance 

_a<sup>&Sigma;</sup>b<sup>&Sigma;</sup>c<sup>&Sigma;</sup>:a<sup>&Gamma;</sup>b<sup>&Gamma;</sup>_

would yield expression of type _&Sigma;\* &times; &Gamma;\*_. This has profound inpact on what can and cannot be a valid expression. For instance 

_ab01ab + 00_

could be a regex that matches string `ab01ab` or string `00`. Suppose however that _&Sigma;={a,b}_ and _&Gamma;={0,1}_. Then the concatenation 

_b<sup>&Sigma;</sup>0<sup>&Gamma;</sup>_

doesn't make much sense anymore. What would be the type of such mixed expression? The same holds for union _b<sup>&Sigma;</sup> + 0<sup>&Gamma;</sup>_. This even allows us to keep track of output alphabet. For instance 
_a<sup>&Sigma;</sup>:0<sup>&Gamma;</sup>_ is of type _&Sigma;\* &times; &Gamma;\*_ and _a<sup>&Sigma;</sup>:a<sup>&Sigma;</sup>_ is of type _&Sigma;\* &times; &Sigma;\*_ but the concatenation  _(a<sup>&Sigma;</sup>:0<sup>&Gamma;</sup>)(a<sup>&Sigma;</sup>:a<sup>&Sigma;</sup>)_ does not have any correct type anymore. This way we can also validate unions and even compositions. For instance _(a<sup>&Sigma;</sup>:0<sup>&Gamma;</sup>)(a<sup>&Sigma;</sup>:a<sup>&Sigma;</sup> &#9675; a<sup>&Sigma;</sup>:1<sup>&Gamma;</sup>)_ would be a valid expression, while _(a<sup>&Sigma;</sup>:0<sup>&Gamma;</sup>)(a<sup>&Sigma;</sup>:a<sup>&Sigma;</sup> &#9675; 1<sup>&Gamma;</sup>:a<sup>&Sigma;</sup>)_ would be illegal. 

There actually is a way to make such interleaved expressions as _b<sup>&Sigma;</sup>0<sup>&Gamma;</sup>_ legal and meaningful. This is in fact a way of creating multi-tape expressions! That is an expression that reads multiple strings simultaneously (and prints several strings in parallel). However, this requires introduction of [local languages](/local_languages.html). 




 
