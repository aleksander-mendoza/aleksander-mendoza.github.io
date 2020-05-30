---
layout: post
title:  "Understandning programming languages"
date:   2020-05-26 01:07:00 +0200
categories: automata
permalink: /understanding_programming.html
---
**Contents**
* TOC
{:toc}
### What is it any why should you care

You know how there are endless discussions about which programming language is better or which snippet of code looks more "elegant". People write entire books about design patterns and your tech lead probably constantly makes negative reviews of your code telling you that you use them wrong. In the eyes of a computer scientist all this is nonsense and here I will try to show you how to think about code like a mathematician. This will really help you a lot in the long run.



### Products, unions, functions

One of the most fundamental data structures in all of computer science is an integer. In school you probably learned that there is this set of natural numbers _&#8469;_ and that _0&isin;&#8469;_, _1&isin;&#8469;_ and so on. Programmers refer to _&#8469;_ as `int` type, but in fact they are equal `int`=_&#8469;_ (ignoring for a moment the limitation of finite computer memory). 

When you write in C you can create `struct` like this:

{% highlight C %}
struct A{
   int x;
   int y;
}
{% endhighlight %}

Mathematician would say _C=&#8469;&times;&#8469;_, because structs are nothing more than Cartesian products. A pair of numbers belongs to set `C` which can be written _(3,5)&isin;C_ but programmer often write `C pair = {3,5}`. More complicated structs are products too. For instance this

{% highlight C %}
struct B{
   int x;
   int y;
   struct Z z;
   struct W w;
}
{% endhighlight %}

is a product _B=&#8469;&times;&#8469;&times;Z&times;W_. Language C also has the structure of `union`. Have you ever wondered why it's named like this? That's right! This is exactly because it __is__ a union! For example the following snippet

{% highlight C %}
union D{
   struct Z z;
   struct W w;
}
{% endhighlight %}

says _D=Z&cup;W_. If _d&isin;Z_ or _d&isin;W_ then _d&isin;D_. Notice how this sentence could have been written  
"_d&isin;Z_ or _d&isin;W_ &rarr; _d&isin;D_"  
and you know how in Haskell, Rust and even Python (with type hints) functions are denoted with `->`? Let's rewrite this sentence by replacing "or" with &cup; as follows:  
"_d&isin;Z&cup;W_ &rarr; _d&isin;D_"  
A programmer would write this as:
{% highlight C %}
D makeD(Z d){ 
   return D{.z=d}
}
D makeD(W d){ 
   return D{.z=d}
}
{% endhighlight %}

Language C does not have function overloading, but in C++ you could write both

{% highlight C %}
Z d = Z{...};
D d2 = makeD(d);
{% endhighlight %}

and

{% highlight C %}
W d = W{...};
D d2 = makeD(d);
{% endhighlight %}

The compiler would figure out which of the overloaded `makeD` functions to use, but from programmer's perspective it looks almost as if the type of `makeD` was _Z &cup; W &rarr; D_. (Also notice how `makeD` resembles a constructor very much. This has special mathematical interpretation, which I will show later.)

This leads us to the third fundamental data structure - functions! That's right! Functions are data structures too. For example

{% highlight C %}
Y f(X x){...}
{% endhighlight %}

is, mathematically speaking, _X&rarr;Y_ and each function are nothing more than a certain subset of _X&times;Y_. One can even denote it as _X&rarr;Y&sub;X&times;Y_. Notice how a programmer could write

{% highlight C %}
/**Use it to store Fibonacci numbers*/
struct F{ 
   int x; //Index of Fibonacci number
   int y; //Value of Fibonacci number
}
{% endhighlight %}

but they might alternatively write this

{% highlight C %}
int f(int x){ 
   if(n == 0)return 0;
   if(n == 1) return 1;
   return f(n-1) + f(n-2);
}
{% endhighlight %}

Every programmer that reads comments around `F` will know that _(x,y)&isin;F_ if and only if _f(x)=y_. However there is nothing that stops the programmer from violating this promise. Whereas `f` doesn't suffer from this problem. Whenever you know _x_ then you can compute the correct _y_. The only problem of `f` is the efficiency. You need to run the function every time you want to access `y`. This is the perfect example of a place where two things are logically equivalent, but differ only in efficiency. If you completely disregared efficiency, then you could indeed store all your data in functions (just as it is in lambda calculus and to some extent in Haskell).

### Contracts on data structures

You should also notice that `F` is not only the data structure _X&times;Y_. It is also the contract  
_(x,y)&isin;X&times;Y &hArr; (x,y)&isin;f_  
In the world of programming there are such contract almost everywhere, but usually you can't see them (There are certain tools that allow you to see the contracts explicitly, like for example [JML](/jml_intro.html)). Sometimes they are written down only in specification like here it is for [Java's equals and hashCode methods](https://docs.oracle.com/javase/7/docs/api/java/lang/Object.html)

> The general contract of hashCode is:

> - Whenever it is invoked on the same object more than once during an execution of a Java application, the hashCode method must consistently return the same integer, provided no information used in equals comparisons on the object is modified. This integer need not remain consistent from one execution of an application to another execution of the same application.
> - If two objects are equal according to the equals(Object) method, then calling the hashCode method on each of the two objects must produce the same integer result.
> - It is not required that if two objects are unequal according to the equals(java.lang.Object) method, then calling the hashCode method on each of the two objects must produce distinct integer results. However, the programmer should be aware that producing distinct integer results for unequal objects may improve the performance of hash tables.

One could say that there are only two kinds of bugs in the world of programming: ones that result from faulty specifications and ones that are caused by breaking (or forgetting) contracts.

### Coproducts

An extremely useful construct in many programming languages is the coproduct. Usually programmers know it under the name of classes.

{% highlight Java %}

interface A { }

class X implements A { }

class Y implements A { }

{% endhighlight %}

Here the set `Y` is the coproduct of `X` and `Y`. In set theory it is often know as disjoin union and is denoted by _Y=X+Y_. Such union is nothing more than _X+Y=X&times;{0} &cup; Y&times;{1}_. In `C` the equivalent way to construct it is using

{% highlight C %}

struct A {
   union{
      struct X x;
      struct Y y;
   };
   /** 
   * if type==0 then use x
   * if type==1 then use y
   */
   int type; 
};

{% endhighlight %}

Of course, this structure is a very simplified version of classes. It lacks abstract methods and inheritance but in essence, all classes are just fancy coproducts with some bells and whistles.

### Tuples

Notice how I gave you example of this function

{% highlight C %}
Y f(X x){...}
{% endhighlight %}

and told you that it's a set _X&rarr;Y_ but you might wonder what about more complex functions like 

{% highlight C %}
Y f(A a,B b,C c){...}
{% endhighlight %}

You should think about them as set of this form _A&times;B&times;C&rarr;Y_. Indeed, notice how much the notation  
`(A a,B b,C c)` resembles a tuple, often found in languages like Python, Haskell, Rust etc. A function with only single argument

{% highlight C %}
Y f(X x){...}
{% endhighlight %}

doesn't in fact take `X x` as input. It takes a unary tuple `(X x)` as input! A function without any arguments

{% highlight C %}
Y f(){...}
{% endhighlight %}

Takes as input a nullary tuple `()`. This is a very interesting case. This tuple is often called the _unit type_ and it has only one element. Just like you can say that _4&isin;int_, you can also write _()&isin;()_. This notation is a bit misleading. It looks as if the set _()_ was a subset of itself but in fact, we just use the same notation to refer both to the set of nullary tuples _()_ and to the only element of this set, which the only nullary tuple _()_ possible.
Notice that when you call such function you don't write

{% highlight C %}
Y y = f;
{% endhighlight %}

Instead you write 

{% highlight C %}
Y y = f();
{% endhighlight %}

because you apply the nullary tuple `()` to `f`. Here the type of `f` is _()&rarr;Y_ which is also the set with only one element
`{((),y)} = f` or alternatively _((),y)&isin;f_.

### Empty sets

You use empty sets in programming all the time without even knowing it. That's mostly because programmers have a different name for it. They call it `void`. Ask yourself, what value is of the type `void`? Absolutely none! You cannot write

{% highlight C %}
void x = 1;
{% endhighlight %}

because _1&notin;&empty;_. You should be careful with void pointer, though. They actually don't have clear mathematical interpretation. `void *` is just a dirty hack to cheat type system and escape all the logic behind programming. 

Just like laws of physics stop working at event horizon, laws of logic stop working at contradition. Mathematicians often refer to it as Duns Scotus law.
It is formally specified as:  
> _&not;x &rarr; (x &rarr; y)_

(for those who don't know:   
_&not;a_ is logical negation "not a".  
_a&rarr;b_ is logical implication "if a then b". )

If _x_ is false, then if at any point you find _x_ to be true, then  _x_ is both true and false, hence you reach contradition. And if contradition is true then everything is true! So you can choose any _y_ and say that it's true. 

For example you could assume _x_ to stand for _1=2_, then _&not;x_ stands for _1&ne;2_. Queen of England and I are two different people. If _1=2_ then 2 people are 1 person. Hence if _1=2_ then I am the Queen of England. When you apply Duns Scotus Law you get 
> _1&ne;2 &rarr; (1=2 &rarr; I am queen of England)_

Anyway, now you are ready to go back to `void` pointers, because they exactly correspond to this law. Every pointer gives you a promise of obtaining value of given time under a specific memory address. However, no value belongs to `void`. So if you dereference such pointer you obtain the impossible. Therefore it makes sense to cast `void *` to any other type of pointer. The expression

{% highlight C %}
void * impossible = ...;
X * x = (X *) impossible;
{% endhighlight %}

corresponds to the logical statement 

> x &notin; &empty; &rarr; ( x &isin; &empty; &rarr; x &isin; X )

and of course _x &notin; &empty;_ always holds true, which leaves us with just

> x &isin; &empty; &rarr; x &isin; X


### Optional types

A very important special case of coproducts are the so called "optionals" (although it's actually mathematical terminology). 
An optional is nothing more than a set _X+()_. You could encode this programmatically more or less as:

{% highlight C %}
struct A {
   union{
      struct X x;
      () y; //this is not valid C
   };
   /** 
   * if type==0 then use x
   * if type==1 then use y
   */
   int type;
};
{% endhighlight %}

but we can simplify it further to

{% highlight C %}
struct A {
   union{
      struct X x;
   };
   /** 
   * if type==0 then use x
   * if type==1 then don't use x
   */
   int type;
};
{% endhighlight %}

However, most of the time you actually don't even need to make such complicated structs. This entire struct `A` is equivalent to just

{% highlight C %}
X * x;  
{% endhighlight %}

because `x` might either be some value of `X` or it might be a null pointer. For this reason every pointer is an optional value of type 
_X+()_.

### Partial functions

Sometimes it happens that certain tasks are impossible. You can't stand up if you are not sitting/laying in the first place. It makes no sense to open the door, which is already open. In the world of mathematics you cannot, for instance, divide by 0 or take derivative of nondifferentiable function. All such phenomena can be described using partial function. 

In the world of programming, partial functions usually have one of two forms: functions that throw exceptions, or functions that crash the whole system. Consider this code

{% highlight Java %}
int[] addArraysElementwise(int[] x, int[] y){
    if(length(x)!=length(y))throw new IllegalArgumentException();
    int[] z = new int[length(x)];
    for(int i=0;i<lenght(z);i++){
    	z[i] = x[i]+y[i];
    }
    return z;
}
{% endhighlight %}

This function returns reult only when arrays `x` and `y` are of the same size. Here is another partial function, but this time it's less obvious that it's partial

{% highlight C %}
int * addArraysElementwise(int * x, int * y){
    int[] z = new int[length(x)];
    for(int i=0;i<lenght(z);i++){
    	z[i] = x[i]+y[i];
    }
    return z;
}
{% endhighlight %}

This version will cause segmentation fault and crash the application if you provide don't use it properly. 

In mathematics partial function is defined as any subset of total function. All the "standard" mathematical functions are total (they return some value for all arguments). 

### Relations

Partial functions are a generalization of total functions. Relations are even farther generalisation of partial functions. Here are their (almost) mathematical definitions.

Function:

> function _X &rarr; Y_ is a subset of _X &times; Y_ such that to every element of _X_ assigns exactly one element of _Y_

Partial function:

> partial function _X &#8640; Y_ is a subset of _X &times; Y_ such that to some elements of _X_ assigns exactly one element of _Y_

Alternative formulation of partial function:

> partial function _X &#8640; Y_ is a subset of _X &times; Y_ such that to all elements of _X_ assigns one or no elements of _Y_

Relation:

> relation _X &times; Y_ to some elements of _X_ assigns some elements of _Y_

Programmatically one could express relations as a function that returns an array (or hash set and similar)

{% highlight C %}
Y[] f(X x){...}
{% endhighlight %}

For example this is how most of the database (`SELECT * FROM Y WHERE X=x`) and filesystem (`ls x`) queries work. 

An alternative but very important representation of relations are function of the form

{% highlight C %}
boolean f(X x, Y y){...}
{% endhighlight %}

They return `true` if the two elements are related and `false` otherwise. For example the equality test `==` is a relation on integers.

When it comes to partial functions, there is also one more alternative way of representing them (instead of crashes and exceptions). This representation stems from that alternative definition that was provided above. Namely, every function of type _X &rarr; Y+()_ can be interpreted as partial. This is exactly what Rust went for in their language. 

{% highlight Rust %}
fn f(X x) -> Option<Y>{...}
{% endhighlight %}

In C one might often write this instead:

{% highlight C %}
Y * f(X x) {...}
{% endhighlight %}


### Recursive data structures

A very important kind of data structures are the ones that contain themselves. For example, have you heard the mathematical definition of numbers? It is known under the name of Peano arithmetic and it can be summarised in the following way:

1. `0` is a number
2. if `x` is a number then `x+1` is a number

Notice how I never said that `1` is a number! Indeed here you should not think of `1` as a number. Instead you should think of `+1` as a function that takes a number and returns some new number. Hence, `1` is actually a number after `0`, written as `0+1`. `2` is a number after `0+1`, written as `0+1+1` and so on. A programmer might define it like this:

{% highlight C %}
struct Number{
    Number * n;
} 
{% endhighlight %}

Notice that `Number * n` might be null. We will use this fact to encode number `0`. Hence we obtain



{% highlight C %}
Number * zero = NULL;
Number one = Number{.n=zero};
Number two = Number{.n=one};
Number three = Number{.n=two};
...
{% endhighlight %}

The greatest advantage that comes from recursive data structures, is their inductive nature. You can use inductive mathematical proofs as basis for you code. If you use this superpower wisely, you could write bug-free code but that's a topic for another post (about [calculus of inductive constructions](/cic.html)). For instance here is an inductive proof that two numbers are equal

> 0 is equal 0  
> x+1 is equal y+1 only when x is equal y

Here is the code version of it:

{% highlight C %}
boolean equals(Number * x, Number * y){
   if(x==NULL && y==NULL){
      return true;
   }else{
      return equals(x.n,y.n); 
   }
}
{% endhighlight %}

Notice that this function only returns `true` and in all other cases it just crashes. It might look impractical but it's "correct". Our inductive definition only defined when two numbers are equal. It never said that some numbers might not be equal or even that there is such a concept as inequality. In other words, this definition describes a partial function. Of course, you can try to make a different definition that would be total and tell whether numbers are equal or inequal. I leave it as a little challenge for the reader.  

### Other data structures

Almost everything in a programming has some mathematical interpretation. Here is a short list of other interesting insights:

- every array `X[]` is a partial function _&#8469;&#8640;X_
- every hash map `{X:Y}` is a partial function _X;&#8640;Y_
- every function `Y f(X x)` that uses global variable `Z z` is actually not of the type _X&rarr;Y_ but instead it is implicitly _X&times;Z&rarr;Y_. 
- every function `Y f(X x)` that uses assigns variable `Z z` is actually not of the type _X&rarr;Y_ but instead it is implicitly _X&rarr;Y&times;Z_. 
- all functions with side-effects correspond to monads (like in Haskell). 
- a function that returns pointer to another function can be thought of as a higher order function _X&rarr;(Y&rarr;Z)_ or simply _X&rarr;Y&rarr;Z_ (these two notations are equivalent)
- a function that takes a pointer to another function can be thought of as a higher order function _(X&rarr;Y)&rarr;Z_




