---
layout: post
title:  "Automata operating on real numbers"
date:   2020-06-03 01:08:00 +0200
categories: automata
permalink: /real_automata.html
---
**Contents**
* TOC
{:toc}

### Introduction

It is possible to extend the definition of finite state automata and enable them to work on real numbers. Suppose that _&Sigma;_ is the alphabet of a finite state automaton and that _&Sigma;=&#8477;_. However, the number of states should still remain as a finite set _Q_. (If we allowed for infinitely many states, then we might as well encode all possible execution paths of Turing machines in them. In fact, we could encode there even more than any Turing machine could ever compute. Therefore it's not very interesting to operate on infinite states.) 
The transition function is defined the same as usual, that is _&delta;:Q &times; &Sigma; &rarr; Q_. Since &Sigma; is infinite, the number of transitions becomes infinite too. If you tried to draw graphs for such automata, you would notice that their edges, start to resemble continuous "clouds" rather than straight lines.

![uncountable_edges](/assets/uncountable_edges.png)

Drawing it this way is very cumbersome. Instead we should draw just one edge and label it with some subset _X&sub;&Sigma;_. 

![uncountable_edge_sets](/assets/uncountable_edge_sets.png)

For instance when _&Sigma;=&#8477;_ then _X&sub;&Sigma;_ might be open interval between _0.5_ (exclusive) and _1_, while Y might be interval between _0_ and _0.5_ (inclusive). We don't need to limit ourselves to intervals, we can use any subsets. 

### Computable real numbers

Almost all real numbers are uncomputable, that is, they cannot be arbitrarily approximated by any algorithm (an intereting and famous example of uncomputable real number is the value of probability that any randomly picked Turing machine will halt). However, some reals can be approximated. There are many different (and largely equivalent) formal definitions of computable real numbers. For our purposes we will only focus on Dedekind cuts. 

Suppose that you pick some real number _x &isin; &#8477;_. You cannot store _x_ in computer's memory, because that would require infinite memory resources. Instead your task is to only build a procedure that takes some rational number and returns boolean value: true if the input number is larger than _x_, false if it's smaller. Below you can see graphical representation of it for the square root of 2.

![dedekind_sqrt_2.png](/assets/dedekind_sqrt_2.png)

For practice, let's try to build Dedekind cut procedure for &pi;. From Taylor Series it's possible to derive the following approximation of &pi;

![pi_approx](/assets/pi_approx.png)

The procedure for Dedekind cut of &pi; will therefore look like this:

    //return true if n>pi
    boolean pi_dedekind_cut(fractional n){
        m = n/4  // this gets rid of the 4 in front of series
        i = 1 // this keeps track of denominator in series
        pi = 1 // this is the upper bound on quarter of pi
        loop forever{
            if m > pi{
                return true
            }
            i = i+2
            pi = pi - 1/i 
            // now it is the lower bound on quarter of pi
            if m < pi {
                return false
            } 
            i = i+2
            pi = pi + 1/i
            // now it is the upper bound on quarter of pi
        }
        //after enough repetitions
        //the loop must eventually exit
    }

If you want to build finite state automata that operate on real numbers, then you have two choices: 

- either you accept the fact that most reals are uncomputable and you will never be able to build simulation of your automata.
- or you restrict all the automata to only operate on computable real numbers

If you choose the second option, then you could encode open intervals in &#8477; with two Dedekind cut procedures. For instance, in order to check if _x &isin; (e,&pi;)_ you would need to run Dedekind cut procedure for _e_ then for _&pi;_ and from this you could find out whether _x_ is between the two reals.

    boolean is_between_e_and_pi(fractional x){
        return e_dedekind_cut(x) and not pi_dedekind_cut(x)
    }


### Computable sets of real numbers 

We can take the notion of computable real numbers one step further and introduce computable subsets of &#8477;

> 1. For every computable real number x &isin; &#8477;, the interval (x,&infin;) is a computable set 
> 2. If X &sub; &#8477; is a computable set then complement of closure (of limit points) of X is a computable set
> 3. If X,Y &sub; &#8477; are computable sets then their union is a computable set
> 4. If X,Y &sub; &#8477; are computable sets then their intersection is a computable set
> 5. &#8477; is a computable set

The first axiom follows directly from definition of Dedekind cut. Axioms 3-4 correspond directly to logical operations of conjunction and disjunction, which are effective operations. The second axiom, corresponds to logical negation but it has a catch - even though we can approximate any real number with arbitrary precision, we can never truly reach it. Hence, negation of Dedekind cut doesn't follow the rule of excluded middle. For instance `pi_dedekind_cut` corresponds to interval (&pi;,&infin;) but the procedure

    boolean pi_dedekind_cut_complement(fractional x){
        return not pi_dedekind_cut(x)
    }

does **not** correspond to (-&infin;,&pi;]. Instead `pi_dedekind_cut_complement` describes interval (-&infin;,&pi;). In fact, the only computable closed set are the trivial ones: &empty; and &#8477;.

### Topology

Suppose that _&Sigma;={0,1,2,3}_ and you want to create a transition over _{0,1,2,3}_. You can do this:

![fsa_transition_acd](/assets/fsa_transition_0_1_2_3.png)

which is equivalent to:

![fsa_transition_acd_set](/assets/fsa_transition_0123.png)

However, in some cases, these two notions are not equivalent! This is especially true, when you decide to optimise the automata. For example, instead of checking

    if(input_symbol == 0){ go to state q3 }
    if(input_symbol == 1){ go to state q3 }
    if(input_symbol == 2){ go to state q3 }
    if(input_symbol == 3){ go to state q3 }

you might perform only two checks 

    if(0 <= input_symbol <= 3){ go to state q3 }

In case of automata that work with real numbers, such appraoch is the only effective one (we cannot check all real numbers with `if` statements). Unfortunately, such nuances cannot be grasped with the standard definition of &delta; transition function. Here we need the strictly graph-theoretic approach.


To better formalize it, we need to use topology. Let _&tau; &sub; 2<sup>&Sigma;</sup>_ be a set of "primitive" subsets of &Sigma;. So for example, if we allow for efficient testing of symbol ranges from &Sigma;, then &tau; contains all those ranges. 
(In particular, if _&Sigma;=\{0,1,2,3\}_ then _&tau;=\{\{\}, {0}, {1}, {2}, {3}, {0,1}, {1,2}, {2,3}, {0,1,2}, {1,2,3}, {0,1,2,3}\}_. Notice that sets that are not ranges, are not in &tau;. For instance _{0,3} &notin; &tau;_.) 

Now, we define finite state automata to be directed multigraphs such that all edges are labeled with "primitive" subsets. Deterministic automata are a special case of finite state automata, such that all edges coming out of a given vertex as mutually disjoint and their union equals &Sigma; (in other words, the edges induce some partition of &Sigma;). Moreover, some vertices are accepting and there is one special vertex which is the initial one. A string is accepted if and only if there is a path that starts in initila state, ends in accepting state and consecutive symbols in that string are members of "primitive" subsets on lebels of consecutive edges in the path. Notice that edge which is labeled with &empty; is useless, because no symbol is a member of it (hence, you should not think that &empty; works like &epsilon;-transition. There are no &epsilon;-transition in this definition).

As an example, consider _&Sigma;=\{0,1,2,3\}_ with topology of ranges _&tau;=\{\{\}, {0}, {1}, {2}, {3}, {0,1}, {1,2}, {2,3}, {0,1,2}, {1,2,3}, {0,1,2,3}\}_. This means that the following is allowed:

![fsa_transition_0_23](/assets/fsa_transition_0_23.png)

But this is not allowed despite giving equivalent resutls: 

![fsa_transition_023](/assets/fsa_transition_023.png)

This definition opens door to many new possibilities. For instance, one could now measure size of automaton in the number of edges and investigate, how the topology &tau; on &Sigma; influences sizes of automata. This is also a much better definition for working with automata over computable sets of real number.

To all those readers who already know topology: primitive subsets are indeed open sets.

#### Binary search transitions

If you define _&tau;_ to be set of all ranges over &Sigma; then it's possible to derive an extremely efficient procedure for transition function &delta;. First, for every vertex, collect all the outgoing edges and store them in a special array. For example let's say that _&Sigma;=[0,1]_ and there is a vertex q<sub>1</sub> like below

![fsa_transition_ranged](/assets/fsa_transition_ranged_reals.png)

It has 4 edges labeled with open intervals. Let's given them names A=(0,0.13), B=(0.12,0.76),  C=(0.32, 0.57), D=(0.45, 0.89) for simplicity (in practice, instead of names, those would be pointers to data structures contatining edges). Notice that some of those intervals overlap, which means that the automaton is nondeterministic. For instance here you can see a graphical representation of all overlapping regions:

![fsa_transition_ranged](/assets/range_axis_full.png)

We can build a very efficient data structure for looking up intervals for any given point. First we need to build 3 arrays. `points` array containing all the points in increasing order. It should also contain the supremum (maximal) element of alphabet _sup(&Sigma;)_. (In this case it's _1=sup(&Sigma;)_ at the end of array.) Second two arrays `begin` and `end` must contain sets of intervals that begin and end respectively at each of the point.


     points = [0, 0.12, 0.13, 0.32, 0.45, 0.57, 0.76, 0.89, 1]
     begin  = [{A}, {B}, {},  {C}, {D},  {},    {},   {},  {}]
     end    = [{},  {},  {A}, {},  {},   {C},   {B},  {D}, {}]

Having this, we can now compute array of intersections `before`. For each point `points[i]`, it should contain set of intervals that intersect (`points[i-1]`,`points[i]`).

    before = [{}, {A}, {A,B}, {B}, {B,C}, {B,C,D}, {B,D}, {D}, {}]
    
The `before` array can be computed by iterating elements of `begin` and `end` simultaneously.

At this point our data structure is ready. The only thing left is the procedure for computing &delta;. Let's say that the automaton reached the vertex q<sub>1</sub> and the next input symbol is some real number _x_. Then we need to find the leftmost index `i` such that `x <= points[i]`. 

    int binary_search(real x){
        fractional i = length(points)/2
        fractional jump =  length(points)/4
        while(floor(i - jump) != floor(i + jump)){
            real point = points[floor(i)]
            if(x <= point){
                i = i - jump
            } else {
                i = i + jump
            }
            jump = jump/2
        }
        real point = points[floor(i)]
        if(x <= point)return floor(i)
        return floor(i)+1 
    }

(Of course notice that `real` is not a computable data structure. In practice, instead of real numbers, we should use fractionals and instead of `<=`, we would use Dedekind cuts.)

Having such leftmost index `i`, we can now lookup array of `before` and `begin` to quickly find out what edges contain `x`.

#### Efficient powerset constructions

Note: make sure you already know what powerset construction is.


If you attempted to perform powerset construction of automaton which works on computable sets of real numbers, you would promptly notice that the traditional methods no longer work. The standard procedure goes like this:

> Treat every subset of Q like a state of new automaton. In order to compute transitions coming out of a given state q &sub; Q, you need to iterate all elements &sigma; &isin; &Sigma; and for every k &isin; q perform transition k' = &delta;(k,&sigma;). After you find all the elements k' for a given &sigma; &isin; &Sigma;, their union will form a new subset of q' &sub; Q, which defines transition &delta;'(q,&sigma;)=q'.

The most difficult task lies in the part "iterate all elements of &Sigma;". If &Sigma; is uncountable, it cannot be done. However, if topology &tau; on &Sigma; consists only of open intervals, then you can use the same approach as in the previous subsection. If you look at this graph again:

![fsa_transition_ranged](/assets/range_axis_full.png)

you may notice that it's not necessary to check all the real numbers. Most of the time, the &delta; transition gives the same outputs. The only points that we really need to check, are those real numbers at the ends and beginnings of intervals. In this particular example, it's enough to only check the points

     points = [0, 0.12, 0.13, 0.32, 0.45, 0.57, 0.76, 0.89, 1]
     
More precisely, you need to check:

- every point `points[i]`
- one arbitrary point between every `points[i]` and `points[i+1]`. You might for instance only check points in the middle of intervals using the formula `(points[i] + points[i+1])/2`.

If &Sigma; is finite, but topology &tau; still consists of ranges, then you can also apply this approach. It may be simplified to checking

- every symbol `points[i]`
- the symbol right before `points[i]` (assuming that you can find a symbol before)



#### Implications for regular expressions

Everything we discussed in this section was in fact a foundation for very efficient extension of regular expressions. UNIX standard allows users to use ranges like `[a-z]` or `[0-9]` etc. If you attempt to compile such expressions to finite state automata, you will notice that creating a hundred transitions for all symbols from range `[ -}]` is a terrible waste of resources (in ASCII space has index 32 and curly brace has index 125. Imagine what would happen if one used UNICODE to create a range spanning thousands of symbols). 

All the algorithms that work for automata over real numbers, will also work for standard automata with finite alphabets. The optimisations we introduced to deal with &#8477;, can be reused to better optimise UNIX-style ranges of symbols.


### Measure theory and probability


The topology &tau; on &Sigma; can be a basis for Borel algebra. If you don't know what Borel algebra is, don't worry. I will only present the most important basics here. The key idea is that if we have Borel algebra, then we can measure "size" of transitions. For instance in this automaton

![fsa_transition_ranged](/assets/fsa_transition_ranged_reals.png)

the edge labeled `0.12 ... 0.76` could have a size equal \|0.76-0.12\| = 0.64. 

Edges like this are indeed weighted. At this moment there is no special significance to those weights. They don't have any impact on the computation itself. However, there is a special case of great importance. That is when the size of alphabet itself if equal \|&Sigma;\|=1. Then we can treat &Sigma; as probabilistic event space, and every element of &Sigma; can be treated like a random event.
Every string in &Sigma; becomes a sequence of random variables. 

If automaton is deterministic (edges of any vertex induce a partition of &Sigma;), then we can speak of probability that a particular edge will be taken in the next transition. For instance, the following is a deterministic probabilistic automaton:

![uncountable_probabilistic](/assets/uncountable_probabilistic.png)

You start in state q<sub>1</sub> then you randomly pick a number between 0 and 1. There is 10% chance of transitioning to q<sub>3</sub> and 90% chance of staying in q<sub>1</sub>. This automaton is indeed a Markov chain! 

![uncountable_probabilistic_markov](/assets/uncountable_probabilistic_markov.png)


Note: The alphabet &Sigma; doesn't always have to be the set [0,1]. It could be any measurable space. I only chose [0,1] because it looks nice and simple on the graph.

#### Relation with multitape automata

I already made a post on [multitape automata](/local_languages.html) but now when we discussed automata over real numbers, we can extend it one step further. Suppose you build an automaton that reads two tapes: one tape has finite alphabet &Sigma;, while the other tape has uncountable alphabet [0,1]. If the automaton is deterministic, then each state must have transitions that jointly cover the whole set _&Sigma; &times; [0,1]_. For instance assume that &Sigma;={a,b}, then the following is a multitape deterministic automaton:

![uncountable_probabilistic_multitape](/assets/uncountable_probabilistic_multitape.png)


At this point you should notice something beautiful. Probabilistic automata **are** a special case of multitatpe automata with one tape reading real numbers (or any measurable set).







