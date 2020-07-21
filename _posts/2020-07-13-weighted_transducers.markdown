---
layout: post
title:  "Weighted Mealy machines"
date:   2020-07-12 01:07:00 +0200
categories: automata
permalink: /weighted_transducers.html
---
**Contents**
* TOC
{:toc}

As I already showed [here](/local_languages.html#multitape-automata), Mealy machines and weighted machines are nothing more than multitape automata. Therefore, weighted Mealy machines are like multitape automata operating on 3 tapes: one for input, one for output and one for weight. In this post, we will consider all machines to be the tuple (Q,q<sub>0</sub>,&Sigma;,&Gamma;,&delta;,F,W) where  
Q is the set of states,  
q<sub>0</sub> is the initial state,  
&Sigma; is the input alphabet (first tape),  
&Gamma; is the output alphabet (second tape),  
_&delta;:Q&times;&Sigma;&rarr;2<sup>Q&times;&Gamma;*&times;W</sup>_ is the transition function,  
F&sub;Q are the accepting states,  
W is the set of weights. 

These machines might accept many different strings &Gamma;* each with their own weight. Ultimately, there should be some criterion for choosing only one output string with the "best" weight.



### Lexicographic weights

This this case, we have some specific alphabet of digits &Omega; and the weights are nothing more than strings _W=&Omega;\*_. The set &Omega; is totally ordered and it gives rise to lexicographic order on _W_. Moreover, we assume that each transition has only single digit as its weight _&delta;:Q&times;&Sigma;&rarr;2<sup>Q&times;&Gamma;*&times;&Omega;</sup>_ instead of _&delta;:Q&times;&Sigma;&rarr;2<sup>Q&times;&Gamma;\*&times;W</sup>_. This way we can ensure that for every triple _(x,y,w)&isin;&Sigma;*&times;&Gamma;*&times;&Omega;*_ accepted by the automaton, the _x_ and _w_ have equal lengths _|x|=|w|_. 

For instance, let _&Omega;={0,1}_, _&Sigma;={a}_, _&Gamma;={u,v}_ and consider the machine

![mealy_weighted_lexicographic](/assets/mealy_weighted_lexicographic.png)

It works in the following way:

<table>
<thead>
  <tr>
    <th>input</th>
    <th>q<sub>0</sub></th>
    <th>q<sub>1</sub></th>
    <th>q<sub>2</sub> (accepting)</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>&epsilon;</td>
    <td>(&epsilon;,&epsilon;,&epsilon;)</td>
    <td></td>
	 <td></td>
  </tr>
  <tr>
    <td>a</td>
    <td></td>
    <td>(a,uv,1)</td>
	 <td>(a,&epsilon;,1)</td>
  </tr>
  <tr>
    <td>aa</td>
    <td>(aa,u,10)</td>
    <td></td>
	 <td>(aa,uvvv,10)</td>
  </tr>
  <tr>
    <td>aaa</td>
    <td>(aaa,uvvvu,100)</td>
    <td>(aaa,uuv,101)</td>
	 <td>(aaa,u,101)</td>
  </tr>
  <tr>
    <td>aaaa</td>
    <td>(aaaa,uu,1010)</td>
    <td>(aaaa,uvvvuuv,1001)</td>
	 <td>(aaaa,uuvvv,1010),(aaaa,uvvvu,1001)</td>
  </tr>
</tbody>
</table>

Notice how nondeterminism arised at input _aaaa_. Automaton can either accept uuvvv with weight 1010 or uvvvu with weight 1001. We use lexicographic ordering to disambiguate nondeterministic outputs. Therefore the automaton accepts uvvvu because 1010 < 1001 (rightmost symbol is the most significant). 

##### Weight elimination

Lexicographic automata have the special property (not enjoyed by other weighted models) that all weights can be safely erased. The only tradeoff is that we might need to add (exponentially) more states.
The conversion algorithm is based on powerset construction. In particular, let (Q,q<sub>0</sub>,&Sigma;,&Gamma;,&delta;,F,W) be the lexicographic weighted automaton, then (2<sup>Q</sup>&times;Q,({q<sub>0</sub>},q<sub>0</sub>),&Sigma;,&Gamma;,&delta;',F') is the equivalent automaton without weights. Every element _(k,q)&isin;2<sup>Q</sup>&times;Q_ represents a superposition of states _k_ with "focus" on _q&isin;k_. We need to "focus" on one representative from _k_, because we need to store the output associated with it. 

For instance, here is the powerset construction of the weighted automaton above

![mealy_weight_eliminated0](/assets/mealy_weight_eliminated0.png)

Every state in this automaton represents some superposition of states from Q. For example _q<sub>0</sub>Q<sub>1</sub>q<sub>2</sub>_ stands for _({q<sub>0</sub>,q<sub>1</sub>,q<sub>2</sub>},q<sub>1</sub>)&isin;2<sup>Q</sup>&times;Q_. Uppercase is used to indicate focused state.

More formally, for every _(k,q)&isin;2<sup>Q</sup>&times;Q_ and _x&isin;&Sigma;_ we perform the image _k'=&delta;(k,x)_ and define  
_&delta;'((k,q),x) = { ((k',q'),y) : (q',y,w) &isin; &delta;(q',x) }_  

Once we have the powerset construction ready, we can move on to the next step of conversion and erase all those transitions that would never be taken due to weights.

![mealy_weight_eliminated1](/assets/mealy_weight_eliminated1.png)

In this case we eiliminated transition from _q<sub>0</sub>Q<sub>1</sub>q<sub>2</sub>_ to _q<sub>0</sub>q<sub>1</sub>Q<sub>2</sub>_ because it had weight 0, while at the same time there was another conflicting transition with larger weight 1, comming to _q<sub>0</sub>q<sub>1</sub>Q<sub>2</sub>_ from _Q<sub>0</sub>q<sub>1</sub>q<sub>2</sub>_. Notice that two states (in this case _q<sub>0</sub>Q<sub>1</sub>q<sub>2</sub>_ and _Q<sub>0</sub>q<sub>1</sub>q<sub>2</sub>_) are conflicting because they both have the same superposition (in this case _{q<sub>0</sub>,q<sub>1</sub>,q<sub>2</sub>}_) and come to the same state (in this case _q<sub>0</sub>q<sub>1</sub>Q<sub>2</sub>_) over the same symbol (in this case _a_).

This concludes the conversion algorithm. Notice that if weighted automaton is functional, then the weight-free automaton will also be functional. Moreover, every weight-free functional automaton can be converted into weighted one with all weights distinct. 

##### Pessimistic case



### p-adic weights




### Probabilistic weights


### Algebraic approach

