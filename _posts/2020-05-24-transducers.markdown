---
layout: post
title:  "Automata with output"
date:   2020-05-24 01:06:00 +0200
categories: automata
permalink: /transducers_intuitively.html
---

**Contents**
* TOC
{:toc}
Note: This post assumes you already have basic understanding of finite state machines and formal languages.


### Mealy and Moore machines for sequential circuits

Consider an example of finite state automaton as below

![fsa_01star](/assets/fsa_01star.png)

In such simple form it's capable of recognizing binary strings like `01`, `0101`, `010101` and so on (including empty string). This can be described using equivalent regular expression `(01)*`.

Now, consider Mealy machine

![mealy_and_gate.png](/assets/mealy_and_gate.png)

The most significant difference is that instead of having accpeting states, this automaton returns output on every edge. This is denoted using `x:y` where `x` symbolizes input and `y` is for output . This particular example implements logical AND gate. It works as follows: 
<table>
<thead>
  <tr>
    <th>state</th>
    <th>input</th>
    <th>output</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>q<sub>0</sub></td>
    <td>&epsilon;</td>
    <td>N/A</td>
  </tr>
  <tr>
    <td>q<sub>1</sub></td>
    <td><ins>1</ins></td>
    <td>0</td>
  </tr>
  <tr>
    <td>q<sub>2</sub></td>
    <td><ins>11</ins></td>
    <td>1</td>
  </tr>
  <tr>
    <td>q<sub>2</sub></td>
    <td>1<ins>11</ins></td>
    <td>1</td>
  </tr>
  <tr>
    <td>q<sub>0</sub></td>
    <td>11<ins>10</ins></td>
    <td>0</td>
  </tr>
</tbody>
</table>

The output is the logical AND of last two digits of input. Notice how in the first row of this table, the output is missing. This is because originally Mealy introduced this model of computation to represent sequential cuircuits equipped with a clock. The output happens after the tick of the processor clock. Hence at the beginning there is not yet any output to be generated. 

The situation is different for Moore machines:

![moore_and_gate.png](/assets/moore_and_gate.png)

Here the output is generated in each state, instead of transitions. Therefore the initial output is `0`. Apart from this one difference, the machines are equivalent.

One could notice that all finite state machines could be interpreted as Moore machines such that accepting states return output `1` and non-accepting states return `0`. For example the Moore machine recognizing `(01)*` would become

![moore_01star](/assets/moore_01star.png)


One can also convert Mealy machines to equivalent Moore machines and vice versa. 

- [Mealy -> Moore](https://www.youtube.com/watch?v=-etILQcfgTg&t=451s) 
- [Moore -> Mealy](https://www.youtube.com/watch?v=HEVWx4irOx4&t=268s) 

### Mealy and Moore machines in theory of formal languages

The original definitions created by Mealy and Moore were very useful in hardware engineering. Unfortunately there are some problems if one tries to use them in to study formal languages. The clocking mechanism doesn't exist in automata theory. Lack of accepting states, makes it unclear how to interpret languages recognized by such models. This further complicates the operations of concatenation, Kleene closure and union. 

One way to define language recognized by Moore machine is by accepting only those strings that generate one particular output. Given a Moore machine _M_ and symbol _c_ we can create a finite state machine _F=M/c_ such that _F_ accepts only those strings for which _M_ outputs _c_. For instance, let _M_ be

![moore_and_gate.png](/assets/moore_and_gate.png)

then machine _M/1_ becomes

![moore_and_gate_image_1.png](/assets/moore_and_gate_image_1.png)

and _M/0_ becomes

![moore_and_gate_image_0.png](/assets/moore_and_gate_image_0.png)

For Mealy machines, such approach will not work but we can do something different. Instead of modeling input language, let's try to model both input and output language together. For example consider _M<sub>2</sub>_ to be the automaton from before:

![mealy_and_gate.png](/assets/mealy_and_gate.png)

Instead of only registering the latest output, let's concatenate all of it into a single string like this

<table>
<thead>
  <tr>
    <th>state</th>
    <th>input</th>
    <th>output</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>q<sub>0</sub></td>
    <td>&epsilon;</td>
    <td>&epsilon;</td>
  </tr>
  <tr>
    <td>q<sub>1</sub></td>
    <td>1</td>
    <td>0</td>
  </tr>
  <tr>
    <td>q<sub>2</sub></td>
    <td>11</td>
    <td>01</td>
  </tr>
  <tr>
    <td>q<sub>2</sub></td>
    <td>111</td>
    <td>011</td>
  </tr>
  <tr>
    <td>q<sub>0</sub></td>
    <td>1110</td>
    <td>0110</td>
  </tr>
</tbody>
</table>

Now the automaon _M<sub>2</sub>_ works like a function that takes string and returns string, hence one may write _M<sub>2</sub> : &Sigma;* &rarr; &Sigma;*_ and _M<sub>2</sub>(&epsilon;)=&epsilon;_, _M<sub>2</sub>(1)=0_, _M<sub>2</sub>(11)=01_ and so on. This "function" is defined on all strings in _&Sigma;*_ (in this case _&Sigma; = {0,1}_ ).

The exact same thing could be done for Moore machines. Consider _M_ again. The "function" _M : &Sigma;* &rarr; &Sigma;*_ would behave a bit differently from _M<sub>2</sub>_ because it returns its output "earlier". Hence _M(&epsilon;)=0_, _M(1)=00_, _M(11)=001_, _M(111)=0011_ and so on. 

One should notice that when viewing those automata as functions on strings, then Mealy and Moore machines are __not longer__ equivalent in power!  Moore machine can return output for empty string _M<sub>2</sub>(&epsilon;)=0_, but there is no way to do the same in Mealy machines. Hence Moore machines have more expressive power. They would be equivalent in power if we didn't count the first output of Moore machines.

### Mealy and Moore machines with accepting states

Let's extend the two models with accepting states and see what happens. Let _M<sub>3</sub>_ be a machine similar to _M<sub>2</sub>_ but with _q<sub>2</sub>_ as sccepting state.

![mealy_and_gate_accepting.png](/assets/mealy_and_gate_accepting.png)

The function _M<sub>3</sub> : &Sigma;* &rarr; &Sigma;*_ is no longer a an actual function, because it's domain is not _&Sigma;*_. In some places there are now "holes". Just like division by zero is not defined, here the output of _M<sub>3</sub>(0)_ is not defined. The only inputs _x_ for which _M<sub>3</sub>(x)_ is defined are those accepted by the underlying atomaton 

![moore_and_gate_image_1.png](/assets/moore_and_gate_image_1.png)

Such functions with some spots missing are called <ins>partial functions</ins> and here we will denote them with &#8640; . Hence we can write _M<sub>3</sub> : &Sigma;* &#8640; &Sigma;*_ and in this example the behaviour can be illustrated with the examples _M<sub>3</sub>(&epsilon;)=&empty;_, _M<sub>3</sub>(1)=&empty;_, _M<sub>3</sub>(11)=01_, _M<sub>3</sub>(111)=011_,  _M<sub>3</sub>(1110)=&empty;. The symbol of empty set &empty; denotes "hole" in function.


The computation with such machines now looks like this:

<table>
<thead>
  <tr>
    <th>state</th>
    <th>input</th>
    <th>output</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>q<sub>0</sub></td>
    <td>&epsilon;</td>
    <td>&empty;</td>
  </tr>
  <tr>
    <td>q<sub>1</sub></td>
    <td>1</td>
    <td>&empty;</td>
  </tr>
  <tr>
    <td>q<sub>2</sub></td>
    <td>11</td>
    <td>01</td>
  </tr>
  <tr>
    <td>q<sub>2</sub></td>
    <td>111</td>
    <td>011</td>
  </tr>
  <tr>
    <td>q<sub>0</sub></td>
    <td>1110</td>
    <td>&empty;</td>
  </tr>
</tbody>
</table>

The exact same thing could be done analogically for Moore machines.

### Mealy and Moore machines with string output

Notice how the length of input and output string are equal in case of 
_M<sub>2</sub> : &Sigma;* &rarr; &Sigma;*_ and _M<sub>3</sub> : &Sigma;* &#8640; &Sigma;*_. This seems to be a very limiting. There are lots of functions that could never be computed with Mealy automata. For example, imagine that you want to use regular expression to replace all occurences of "c" with "cpp". This cannot be done with the previous models. Let's extend the machines once again and allow them to output entire strings. The input on all edges must still remain as single symbol though. Here is an example of Mealy machine _M<sub>4</sub>_ that performs the task of subtitution:

![mealy_c_for_cpp.png](/assets/mealy_c_for_cpp.png)

You can now use it and obtain _M<sub>4</sub>("main.c")="main.cpp"_. The problem with this mautomaton is that it will replace "c" everywhere. It would be a lot more useful if we could replace "c" only if it is at the end of the string. Try to build a better automaton that would change file extensions. For example it should turn "main.c" into "main.cpp", "ccc.c" to "ccc.cpp", "Main.java" to "Main.java", "clojure.clj" to "clojure.clj" and so on.

Read further only if you want to see the spoiler to this task.

Thanks to the ability of returning output of any length from each edge (or state in case of Moore machines), the expressive power got radically greater. However, there is a certain important property that all the automata have in common. Consider any of the previous Mealy machines. It might be augmented with accepting states like _M<sub>3</sub>_. It might even produce strings on transition output like _M<sub>4</sub>_. Notice that no matter which model of Mealy machine you choose, they all start their computation in some initial state and then they can take only one transition for each character in input (they are all deterministic). For example something like this is __not__ allowed:

 ![mealy_nondet.png](/assets/mealy_nondet.png)
 
If we did allow for such nondeterminism, then the automaton would return output _y<sub>0</sub>=1_ for input _x<sub>0</sub>=0_ and _y<sub>1</sub>=01_ for _x<sub>1</sub>=00_. Notice that the string  _x<sub>0</sub>_ is a prefix of _x<sub>1</sub>_ but the output _y<sub>0</sub>_ is not a prefix of _y<sub>1</sub>_! For all the models described above, it indeed holds that whenever one input is prefix of another, then the respective output is the prefix of another. We shall refer to this as _rule of matching prefixes_. If we can find an example violating this rule, then we can prove that there is no Mealy machine capable of computing given function. This indeed is the case in the previous task. "ccc.c" should be mappted to "ccc.cpp", but "clojure.clj" is not mapped to "clojure.cpplj". Instead  "clojure.clj" is mapped to "clojure.clj". Hence the task is impossible to solve without introducing a new model of Mealy machines.



### Mealy and Moore machines with multiple output tapes


TODO


### Nondeterministic Mealy machines

Multiple output tapes allow us to break the rule of matching prefixes but it comes at the expense of greatly complicating the model and there are still many cases in which it's not expressible enough. A better alternative is the introduction of nondeterminism (string output from transitions is not allowed at this moment).

Let's illustrate how it works with an example

 ![mealy_nondet_multi_output.png](/assets/mealy_nondet_multi_output.png)


<table>
<thead>
  <tr>
    <th>states:outputs</th>
    <th>input</th>
    <th>returned output</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>q<sub>0</sub>:&epsilon;</td>
    <td>&epsilon;</td>
    <td>&empty;</td>
  </tr>
  <tr>
    <td>q<sub>1</sub>:1, q<sub>2</sub>:0</td>
    <td>0</td>
    <td>1</td>
  </tr>
  <tr>
    <td>q<sub>4</sub>:10, q<sub>3</sub>:01</td>
    <td>00</td>
    <td>01</td>
  </tr>
  <tr>
    <td>q<sub>5</sub>:{010,101}</td>
    <td>001</td>
    <td>010, 101 </td>
  </tr>
  <tr>
    <td>&empty;</td>
    <td>0011</td>
    <td>&empty; </td>
  </tr>
</tbody>
</table>

A string is accepted whever there exists a path that starts in initial state, ends in accepting state and labels on the transitions correspond to consecutive symbols in the string. Then you can collect all the outputs produced along the way and concatenate them together to obtain output for a given input string. There might be more than one path possible. In such cases all viable paths are taken into account, and the machine could produce multiple outputs. This situation is illustrated with state  _q<sub>5</sub>_, which can be reached in two different ways, producing two different outputs.
Nondeterministic Mealy machines which never produce more than one output are called __functional__. It's a special subset of machines that has many interesting properties and we shall discuss it in more depth later.

#### Epsilon transitions

Notice how in this model all outputs and inputs have equal length. One way to extend the power is by allowing strings on transition outputs, but in case of nondeterministic machines, there is one more way to achieve the same result. That is by introducing &epsilon;-transitions.

 ![mealy_nondet_epsilons.png](/assets/mealy_nondet_epsilons.png)

Here it works like this:


<table>
<thead>
  <tr>
    <th>states:outputs</th>
    <th>input</th>
    <th>returned output</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>q<sub>0</sub>:&epsilon;, q<sub>6</sub>:1, q<sub>7</sub>:10</td>
    <td>&epsilon;</td>
    <td>1, 10</td>
  </tr>
  <tr>
    <td>q<sub>1</sub>:1, q<sub>2</sub>:0, q<sub>3</sub>:00</td>
    <td>0</td>
    <td>1, 00</td>
  </tr>
  <tr>
    <td>q<sub>4</sub>:10</td>
    <td>00</td>
    <td>&empty;</td>
  </tr>
  <tr>
    <td>q<sub>5</sub>:00</td>
    <td>000</td>
    <td>00</td>
  </tr>

</tbody>
</table>

With such model every string output 

![mealy_string_output](/assets/mealy_string_output.png)

can be simulated using

![mealy_string_epsilon_output](/assets/mealy_string_epsilon_output.png)

There are a couple of very important new features.

1. It finally becomes possible for Mealy machines to return output before reading any input.
2. Every transition that outputs a string, can be simulated with several &epsilon;-transitions that output single symbol. 
3. With help of cycles of &epsilon;-transitions, one can return infinitely many outputs for one input string.

The last point requires special consideration. Consider an example of

![mealy_nondet_inf_output.png](/assets/mealy_nondet_inf_output.png)

The output returned for _0_ is of course _1_, but that's not all! You might also take the epsilon transition and then transition over _0_ to produce _01_. There is nothing that stops you from taking the epsilon transition even more times. In fact all the outputs _1_, _01_, _001_, ... are returned for input _0_. The &epsilon;-loop at state q<sub>0</sub> is indeed an &epsilon;-cycle, that generate infinitely many outputs. In fact every automaton that returns inifinite number of outputs but have an &epsilon;-cycle, but the converse is not true. Is is a counter-example:

![mealy_nondet_single_eps_output.png](/assets/mealy_nondet_single_eps_output.png)

This automaton contains two &epsilon;-cycles but none of them leads to infinitely many outputs. The &epsilon;-loop at q<sub>0</sub> returns &epsilon; and concatenation of any number of epsilons still yields an epsilon. The second &epsilon;-loop is at state q<sub>2</sub>, which unfortunately have any way to reach an accepting state. This state is a dead-end. 

#### Epsilon-elimination

One should notice that all the dead-end states can be safely removed without affecting functionality of automaton. Similarly all the &epsilon;-cycles that do not produce any output other than &epsilon; can be replaced with non-&epsilon;-transitions. For example this:

![mealy_eps_cycle.png](/assets/mealy_eps_cycle.png)

needs to become this:

![mealy_eps_cycle.png](/assets/mealy_eps_no_cycle.png)

In general, removing transitions of the form _&epsilon;:&epsilon;_ works the same as removing &epsilon;-transitions from NFA (nondeterministic finite state automata). Removing those of the form _&epsilon;:y_ is a bit more complex. You can replace every transition like this:

![mealy_eliminable_epsilon](/assets/mealy_eliminable_epsilon.png)

with this:

![mealy_eliminated_epsilon](/assets/mealy_eliminated_epsilon.png)

The only problem is when _q<sub>0</sub>_ is the initial state and _q<sub>1</sub>_ is accepting. Then such operation will change the functionality of the machine. 

The most important conclusion is therefore that nondeterministic Mealy machines with &epsilon;-transitions which:

- don't have &epsilon;-cycles
- don't produce non-empty output for empty input

can be reduced to equivalent &epsilon;-free Mealy machines.

#### Projections

For every Mealy machine (no matter which variant of the model you choose) it's possible to build a finite state automaton that accepts  the input language. We already showed it in one of previous examples. Given a Mealy machine:

![mealy_and_gate_accepting.png](/assets/mealy_and_gate_accepting.png)

you can build finite state machine that accepts input lagnuage:

![moore_and_gate_image_1.png](/assets/moore_and_gate_image_1.png)

The interesting part is that you can do the same for output language:

![moore_and_gate_image_1.png](/assets/moore_and_gate_out_proj.png)

but this time, the resulting finite state machine may be nondeterministic even in cases when Mealy machine was deterministic (notice how there are two transitions over _0_ coming out of state _q<sub>0</sub>_).
This construction is achieved by "erasing" output labels (when building automaton that accepts input language) or input labels (when building automaton that accepts output language). The only complications is when using Mealy machine with entire strings on output labels. It's not possible to just "erase" input labels and call it done. Instead, we need to first replace string transitions with multiple &epsilon;-transitions and only then we can erase the input labels.

### Functional Mealy machines

As mentioned earlier, functional Mealy machines are a special case of nondeterministic Mealy machines that never produce more than one output.

There are two very important properties:

1. such machines cannot contain &epsilon;-cycles (and even if they do contain, then it must be possible to safely eliminate all the cycles)
2. at no point throughout the computation there could be a state with more than one associated output. 

The second point requires more explanation. Consider the example

![mealy_multi_output_intermediate](/assets/mealy_multi_output_intermediate.png)

There is only one transition coming to state _q<sub>2</sub>_ and it might seem like there is no way it could possibly return multiple outputs. In one of the previous example we already showed how multiple transitions can reach the same accepting state simultaneously (Look at one of the previous the tables with row reaching _q<sub>5</sub>:{010,101}_). Here the situation is different.


<table>
<thead>
  <tr>
    <th>states:outputs</th>
    <th>input</th>
    <th>returned output</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>q<sub>0</sub>:&epsilon;</td>
    <td>&epsilon;</td>
    <td>&empty;</td>
  </tr>
  <tr>
    <td>q<sub>1</sub>:{0,1}</td>
    <td>0</td>
    <td>&empty;</td>
  </tr>
  <tr>
    <td>q<sub>2</sub>:{0,1}</td>
    <td>00</td>
    <td>0,1</td>
  </tr>
  

</tbody>
</table>

First the state _q<sub>1</sub>_ is reached and it has 2 outputs associated with it. Then only in the next step, those two outputs reach accepting state  _q<sub>2</sub>_ and both outputs are coming from the same transition! The conclusion is as follows: if at any point it's possible to reach some non-accepting state _q<sub>i</sub>_ from two different paths, yielding two different outputs, then either:

- the state _q<sub>i</sub>_ is a dead-end and will never accept, therefore it might be deleted altogether.
- there is some way for _q<sub>i</sub>_ to reach accepting state, hence the automaton is not functional.

#### Checking if automaton is functional

The task of checking whether given nondetermministic machine is functional, is a coNP-hard problem. The exact proof of it requires understanding of complexity theory, so I will just omit it here. There is however, one thing we can do in order to more efficienlty sieve out some of the cases. The procedure has three steps. First we remove all dead-end states. Second we need to check if there is a state, that has two transitions leading to the same end state over the same symbol and producing different outputs. In the example above this is the case for _q<sub>0</sub>_ which has two transitions leading to _q<sub>1</sub>_. If we find such state, then we know that the automaton cannot be functional. If no such state is found then we proceed further. The third and last step of this test relies on powerset construction. In the example above, the automaton consists of 3 states: _q<sub>0</sub>_, _q<sub>1</sub>_, _q<sub>2</sub>_. There are the following combinations of those states:

- { _q<sub>0</sub>_ } - this is the initial combination
- { _q<sub>1</sub>_ } - combination is obtained from { _q<sub>0</sub>_ } be transitioning over _0_.
- { _q<sub>2</sub>_ } - combination is obtained from { _q<sub>1</sub>_ } be transitioning over _0_.
- { _q<sub>0</sub>_ , _q<sub>1</sub>_} - this combination is unreachable
- { _q<sub>1</sub>_ , _q<sub>2</sub>_}
- { _q<sub>2</sub>_ , _q<sub>0</sub>_}
- {  _q<sub>0</sub>_ , _q<sub>1</sub>_ , _q<sub>2</sub>_}
- {  } - the empty combination of states can be reached from { _q<sub>2</sub>_ } by transitioning over _0_, or from any other combination by transitioning over _1_.

In the tables above you can often find examples that reach combinations of more than one state. Now the test relies on finding all reachable combinations. Then for each letter _x_ of alphabet _&Sigma;*_ we need to check if there exist two states that transition to the same state over _x_. For example here 

 ![mealy_nondet_multi_output.png](/assets/mealy_nondet_multi_output.png)
 
it is possible to reach combination of states _q<sub>4</sub>, q<sub>3</sub>_ that transition to _q<sub>5</sub>_ over _1_. If it's not possible to find such pair of states, then the automaton is funtional. However, if we find such states, it doesn't immediately imply that the automaton is not functional. For example this automaton:

 ![mealy_func](/assets/mealy_func.png)
 
is functional despite the fact, that there are two paths with the same input labels _00_ that lead to state _q<sub>2</sub>_. Both of these paths produce __the same__ output _0_, therefore the automaton is functional.
 
This concludes the procedure for checking if automaton is functional. It's not a full check! It can only detect some cases, while some special scenarios slip through this test (like the example above). But most of the "interesting" and important cases can be detected this way. Performing a full definitive check would require a lot more work and more time.

### Weighted automata

Suppose that you are only interested in functional Mealy machines and you want to find some way of prioritizing certain output over the other. This way, whenever there appears ambiguity/conflict you will just pick one output with higher priority. By adding priority, all nondeterministic automata can be turned into functional Mealy automata (but this of course changes their functionality, so the models are not equivalent). For example this automaton:

 ![mealy_weighted.png](/assets/mealy_weighted.png)

uses alphabet _{a,b}_ but you can see two stransitions that have 3 components. That third component is the weight, and every time the weight is specified directly it is assumed to be zero. For instance _a:b_ actually stands for _a:b:0_. The way this automaton works is as follows:

<table>
<thead>
  <tr>
    <th>states:outputs</th>
    <th>input</th>
    <th>returned output</th>
    <th>comment</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>q<sub>0</sub>:&epsilon;</td>
    <td>&epsilon;</td>
    <td>&empty;</td>
    <td></td>
  </tr>
  <tr>
    <td>q<sub>1</sub>:b, q<sub>2</sub>:a</td>
    <td>a</td>
    <td>&empty;</td>
    <td></td>
  </tr>
  <tr>
    <td>q<sub>3</sub>:aa</td>
    <td>aa</td>
    <td>aa</td>
    <td>q<sub>3</sub> only received output from q<sub>2</sub> because it came through a transition with higher priority 0.5 > 0.3</td>
  </tr>
  <tr>
    <td>q<sub>1</sub>:b</td>
    <td>b</td>
    <td>&empty;</td>
    <td></td>
  </tr>
  <tr>
    <td>q<sub>3</sub>:bb</td>
    <td>ba</td>
    <td>bb</td>
    <td>This time the input came from the transition with lower priority because state q<sub>2</sub> hasn't accumulated any output at this point</td>
  </tr>
</tbody>
</table>

For all those readers who already know something about transducers and probabilistic automata, here is an important note: those weight are not (yet) probabilities. They are not multiplied together and are not accumulated in any way. I will write specifically about probabilistic automata below.

It might seem like adding such weights is another significant extention that increases the power of automata. This is not actually true. Weighted &epsilon;-free Mealy machines are equivalent in power with functional &epsilon;-free Mealy machines. Many weighted Mealy machines with &epsilon;-transitions can also be converted to equivalent automata without weights, but in some cases it's not possible. The exact details are complicated so I omit them here.

You might ask yourself, why do we even bother with weighted Mealy machines, if they don't increase expressive power. There are two reasons:

1. weighted Mealy machines often usually smaller than their corresponding non-weighted counterparts. It can be proved that there exist weighted Mealy machines that are exponentially smaller than their equivalent functional Mealy machines.
2. it's a good introduction to probabilistic Mealy machines

### Hybrid of Mealy-Moore automata

Consider an automaton that is a mix of both Mealy and Moore models. It has outputs on all edges just like Mealy machine **and** is also has Moore-style output on accepting states. 

![mealy_and_moore](/assets/mealy_and_moore.png)

Most of the time this machine works just like Mealy machines. The only difference is that whenever it reaches the accepting state, it should append its output right before accepting.  

<table>
<thead>
  <tr>
    <th>state</th>
    <th>input</th>
    <th>output</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>q<sub>0</sub></td>
    <td>&epsilon;</td>
    <td>&empty;</td>
  </tr>
  <tr>
    <td>q<sub>1</sub></td>
    <td>1</td>
    <td>&empty;</td>
  </tr>
  <tr>
    <td>q<sub>2</sub></td>
    <td>11</td>
    <td>010</td>
  </tr>
  <tr>
    <td>q<sub>2</sub></td>
    <td>111</td>
    <td>0110</td>
  </tr>
  <tr>
    <td>q<sub>0</sub></td>
    <td>1110</td>
    <td>&empty;</td>
  </tr>
</tbody>
</table>


This in fact is equivalent to using &epsilon;-transitions as follows:

![mealy_and_moore_as_epsilon](/assets/mealy_and_moore_as_epsilon.png)

The truly interesting aspect of such hybrid machines becomes visible when we also allow for entire strings on transition outputs. Then the model becomes equivalent in power so functional machines, without requiring use of any &epsilon;-transitions! There is a procedure for converting any functional (nondeterministic with &epsilon;-transitions but without &epsilon;-cycles) Mealy machine to hybrid Mealy-Moore machine. I will explain it below with the examples

#### Converting functional machines

Consider the folowing nondeterministic automaton.

![mealy_func_to_mealy_moore_1](/assets/mealy_func_to_mealy_moore_1.png)

Before starting the procedure, let's first make sure that this automaton is indeed functional. Here is its powerset graph (of reachable combinations):


![mealy_func_to_mealy_moore_powerset](/assets/mealy_func_to_mealy_moore_powerset.png)

Each state of this graph represents several states of the original automaton. For instance the state tagged `0,1,4` corresponds to set of states _q<sub>0</sub>_, _q<sub>1</sub>_ and _q<sub>4</sub>_. There is also blank state which represents empty set. Each transition shows how the nondeterminism is affected by subsequent input. If you analyse this graph you will see that there is no conflict of outputs at any point. For instance  initially the automaton starts in _q<sub>0</sub>_, which immediately transitions to _q<sub>1</sub>_ and _q<sub>4</sub>_ over &epsilon;. Then if the next input is _0_, the state _q<sub>0</sub>_ will be "lost" as it doesn't have any transition for input _0_. The state _q<sub>1</sub>_ will transition to _q<sub>2</sub>_ and _q<sub>3</sub>_. The state _q<sub>4</sub>_ doesn't have any outgoing transitions and will get "lost" too. This yields transition `0 -> ` + `1 -> 2,3` + `4 -> ` = `0,1,4 -> 2,3`. There is no conflict here because none of the states _q<sub>0</sub>_, _q<sub>1</sub>_, _q<sub>4</sub>_ transition to the same next state simultaneously. There would be a conflict, if for example there was `0 -> 2` + `1 -> 2,3` + `4 -> ` = `0,1,4 -> 2,2,3`.

Alright, so now that we know the machine is functional, we can begin converting it to Mealy-Moore hybrid.
In the first stage of convertion procedure, we will try to find all &epsilon;-transitions <ins> that do not come out of initial state </ins> and remove them. Here you can see one such transition between _q<sub>1</sub>_ and _q<sub>4</sub>_. It can be eliminated as follows:

![mealy_func_to_mealy_moore_3](/assets/mealy_func_to_mealy_moore_3.png)

We essentially copied all the edges coming into _q<sub>1</sub>_ and connected them to  _q<sub>4</sub>_, while also appending to them the output returned from the &epsilon;-transition. You can see that while doing so, we had to introduce a new &epsilon;-transition from _q<sub>0</sub>_ to _q<sub>4</sub>_. Thanks to the guarantee that there are no &epsilon;-cycles, we can be sure that we won't be introducing new &epsilon;-transition infinitely forever as we remove the previous ones. (Indeed you can see that graphs without &epsilon;-cycles can only contain  &epsilon;-trees, therefore we will eventually reach the root of the tree, as we progress with removing &epsilon;-transitions).

Now we reached a point when all the remaining &epsilon;-transitions are coming out of initial state. We cannot remove them the same way as before. We need to "split" the initial state into two new states as follows:

![mealy_func_to_mealy_moore_4](/assets/mealy_func_to_mealy_moore_4.png)

The original initial state  _q<sub>0</sub>_ should not have any incoming transitions anymore. Instead the new state  _q<sub>5</sub>_ will now try to mimic the missing functionality of  _q<sub>0</sub>_. (Note: You can see there is some analogy in this approach to expanding Kleene closure from _a\*_ to _(a+&epsilon;)a*_). 
Now let's remove transition between _q<sub>0</sub>_ and _q<sub>1</sub>_.

![mealy_func_to_mealy_moore_5](/assets/mealy_func_to_mealy_moore_5.png)

All the transitions coming out of _q<sub>1</sub>_, now have been duplicated for _q<sub>0</sub>_ too. In the next step let's remove &epsilon;-transition from _q<sub>0</sub>_ and _q<sub>4</sub>_:

![mealy_func_to_mealy_moore_6](/assets/mealy_func_to_mealy_moore_6.png)

This time, because _q<sub>4</sub>_ did not have any outgoing transitions, we did not need to add any new transitions to _q<sub>0</sub>_. However, the most important change was marking _q<sub>0</sub>_ as accepting and adding to it Moore-style output _101_.

At this point there are no more &epsilon;-transitions  coming out of the initial state. We can safely remove all the remaining transitions left in other parts of automaton. Let's get rid of the one from _q<sub>5</sub>_ to _q<sub>4</sub>_:

![mealy_func_to_mealy_moore_7](/assets/mealy_func_to_mealy_moore_7.png)

And the last one from _q<sub>5</sub>_ to _q<sub>1</sub>_:

![mealy_func_to_mealy_moore_8](/assets/mealy_func_to_mealy_moore_8.png)

We can see that the state _q<sub>5</sub>_ was necessary only temporarily but in the final result it's a dead-end state that could be safely removed. 

The final phase of the procedure is to add missing Moore output to all other accepting states:

![mealy_func_to_mealy_moore_9](/assets/mealy_func_to_mealy_moore_9.png)

#### Formal perspective

The mathematical definition of Mealy-Moore hybrid machine is a tuple (Q,i,&Sigma;,&Gamma;,&delta;,F) where

> Q is the set of states  
> i&isin;Q is the initial state  
> &Sigma; is the input alphabet  
> &Gamma; is the output alphabet  
> &delta;:Q&times;&Sigma;&rarr;2<sup>Q&times;&Gamma;\*</sup> is the (nondeterministic) transition function  
> F:Q&#8640;&Gamma;\* is accepting function which returns Moore-style output

Earlier I gave examples where some states were final and they printed Moore-style output before accepting. However, a bit more mathematical approach would be to view all states as final, except some of them print &empty; as Moore-style output. &empty; annihilates all previous output.  For instance:

- when you concatenate strings _a_ and _b_ you obtain string _a&middot;b=ab_, 
- when you concatenate singletone sets of strings _{a}_ and _{b}_ you obtain a new singleton set _{a&middot;b=ab}_, 
- when you concatenate sets of strings _{a,bf}_ and _{ce,&epsilon;}_ you obtain set _{a&middot;&epsilon;=a,bf&middot;&epsilon;=bf,ac&middot;e=ace,bf&middot;ce=bfce}_
- but when you concatenate _{a,b,c}_ with empty set _{}_ you obtain the same empty set as a result.

Hence the example at the beginning of this section, should in fact be viewed as:

![mealy_and_moore](/assets/mealy_and_moore_empty_set.png)

<table>
<thead>
  <tr>
    <th>state</th>
    <th>input</th>
    <th>output</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>q<sub>0</sub></td>
    <td>&epsilon;</td>
    <td>&epsilon;&middot;&empty;=&empty;</td>
  </tr>
  <tr>
    <td>q<sub>1</sub></td>
    <td>1</td>
    <td>0&middot;&empty;=&empty;</td>
  </tr>
  <tr>
    <td>q<sub>2</sub></td>
    <td>11</td>
    <td>01&middot;0=010</td>
  </tr>
  <tr>
    <td>q<sub>2</sub></td>
    <td>111</td>
    <td>011&middot;0=0110</td>
  </tr>
  <tr>
    <td>q<sub>0</sub></td>
    <td>1110</td>
    <td>0110&middot;&empty;=&empty;</td>
  </tr>
</tbody>
</table>

Therefore you can see that this machine always "accepts" and always "returns" output, except that sometimes that output might be a string and sometimes it might be "nothing".


### Probabilistic automata (transducers)

TODO



