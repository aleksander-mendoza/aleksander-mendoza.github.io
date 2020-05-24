---
layout: post
title:  "Automata with output"
date:   2020-05-24 01:06:00 +0200
categories: automata
---


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

![moore_01star_image_1.png](/assets/moore_01star_image_1.png)

and _M/0_ becomes

![moore_01star_image_0.png](/assets/moore_01star_image_0.png)

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

The exact same thing could be done for Moore machines. Consider _M_ again. The "function" _M : &Sigma;* &rarr; &Sigma;*_ would be a bit differently from _M<sub>2</sub>_ because of the it retunrs output "earlier". Hence _M(&epsilon;)=0_, _M(1)=00_, _M(11)=001_, _M(111)=0011_ and so on. 

One should notice that when viewing those automata as functions on strings, then Mealy and Moore machines are __not longer__ equivalent in power!  Moore machine can return output for empty string _M<sub>2</sub>(&epsilon;)=0_, but there is no way to do the same in Mealy machines. Hence Moore machines have more expressive power. They would be equivalent in power if we didn't count the first output of Moore machines.

### Mealy and Moore machines with accepting states

Let's extend the two models with accepting states and see what happens. Let _M<sub>3</sub>_ be a machine similar to _M<sub>2</sub>_ but with _q<sub>2</sub>_ as sccepting state.

![mealy_and_gate_accepting.png](/assets/mealy_and_gate_accepting.png)

The function _M<sub>3</sub> : &Sigma;* &rarr; &Sigma;*_ is no longer a an actual function, because it's domain is not _&Sigma;*_. In some places there are now "holes". Just like division by zero is not defined, here the output of _M<sub>3</sub>(0)_ is not defined. The only inputs _x_ for which _M<sub>3</sub>(x)_ is defined are those accepted by the underlying atomaton 

![moore_01star_image_1.png](/assets/moore_01star_image_1.png)

Such functions with some spots missing are called <ins>partial functions</ins> and here we will denote them with &#8640; . Hence we can write _M<sub>3</sub> : &Sigma;* &#8640; &Sigma;*_ and in this example the behaviour can be illustrated with the examples _M<sub>3</sub>(&epsilon;)=&empty;_, _M<sub>3</sub>(1)=&empty;_, _M<sub>3</sub>(11)=01_, _M<sub>3</sub>(111)=011_,  _M<sub>3</sub>(1110)=&empty;. The symbol of empty set &empty; denotes "hole" in function.

The exact same thing could be done analogically for Moore machines.

### Mealy and Moore machines with string output

Notice how the length of input and output string are equal in case of 
_M<sub>2</sub> : &Sigma;* &rarr; &Sigma;*_ and _M<sub>3</sub> : &Sigma;* &#8640; &Sigma;*_. This seems to be a very limiting. There are lots of functions that could never be computed with Mealy automata. For example, imagine that you want to use regular expression to replace all occurences of "c" with "cpp". This cannot be done with the previous models. Let's extend the machines once again and allow them to output entire strings. The input on all edges must still remain as single symbol though. Here is an example of Mealy machine _M<sub>3</sub>_ that performs the task of subtitution:

![mealy_c_for_cpp.png](/assets/mealy_c_for_cpp.png)

You can now use it and obtain _M<sub>3</sub>("main.c")="main.cpp"_. The problem with this mautomaton is that it will replace "c" everywhere, not only at the end of string. Try to build a better automaton that would change file extensions. For example it should turn "main.c" into "main.cpp", "ccc.c" to "ccc.cpp", "Main.java" to "Main.java", "clojure.clj" with "clojure.clj" and so on.

Read further only if you want to see the spoiler to this task.

Thanks to the ability of returning output of any length from each edge (or state in case of Moore machines). The expressive power got radically greater. However, there is a certain important property that all the automata have in common. Consider _M<sub>0-3</sub>_ to be some Mealy machine. It might be augmented with accepting states like. It might even produce strings from transition output. Notice that no matter which model of Mealy machine you choose, they all start their computation in some initial state and then they can take only one transition for each character in input (they are all deterministic). For example something like this is __not__ allowed:

 ![mealy_nondet.png](/assets/mealy_nondet.png)
 
If we did allow for such nondeterminism, then the automaton would return output _y<sub>0</sub>=1_ for input _x<sub>0</sub>=0_ and _y<sub>1</sub>=01_ for _x<sub>1</sub>=00_. Notice that the string  _x<sub>0</sub>_ is a prefix of _x<sub>1</sub>_ but the output _y<sub>0</sub>_ is not a prefix of _y<sub>1</sub>_! For all the models described above, it indeed holds that whenever one input is prefix of another, then the respective output is the prefix of another. We shall refer to this as _rule of matching prefixes_. If we can find an example violating this rule, then we can prove that there is no Mealy machine capable of computing given function. This indeed is the case in the previous task. "ccc.c" should be mappted to "ccc.cpp", but "clojure.clj" is not mapped to "clojure.cpplj". Instead  "clojure.clj" is mapped to "clojure.clj". Hence the task is impossible to solve without introducing a new model of Mealy machines.



### Mealy and Moore machines with multiple output tapes


TODO


### Nondeterministic Mealy and Moore machines

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

</tbody>
</table>

A string is accepted whever there exists a path that starts in initial state, ends in accepting state and labels on the transitions correspond to consecutive symbols in the string. Then you can collect all the outputs produced along the way and concatenate them together to obtain output for a given input string. There might be more than one path possible. In such cases all viable paths are taken into account, and the machine could produce multiple outputs. This situation is illustrated with state  _q<sub>5</sub>_, which can be reached in two different ways, producing two different outputs.




