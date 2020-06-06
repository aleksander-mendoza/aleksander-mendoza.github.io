---
layout: post
title:  "Encoding and recognizing pairs of strings"
date:   2020-05-31 01:07:00 +0200
categories: automata
permalink: /recognizing_language_relations.html
---
**Contents**
* TOC
{:toc}
  
Mealy machines can be often emulated using other models of computation that do not produce any output. This topic is continuation and extension of posts about [multitape automata](local_languages.html) and [Mealy machines](transducers_intuitively.html)

### Cartesian product alphabets

Every [deterministic Mealy machine](/transducers_intuitively.html#mealy-and-moore-machines-with-accepting-states) is equivalent to two-tape deterministic automaton. You may even notice the striking similarity between these two graphs:

![mealy_and_gate_accepting.png](/assets/mealy_and_gate_accepting.png)
![mealy_and_gate_accepting.png](/assets/multitape_and_gate_accepting.png)

The philosophy behind such machines is a little different. The Mealy machine generates output for given input, whereas multitape automaton reads both input and output and decides whether the input corresponds to given output. One machine "solves the problem" (generates output), the other "verifies the solution" (recognizes output). 

However, the rigorous mathematical definition doesn't see any distinction between these two models. From mathematical perspective Mealy machines **are** multitatpe automata. The same holds for nondeterministic Mealy machines and nondeterministic multitape automata. 


### Pushdown automata

You may also see Mealy machines as a very special kind of pushdown automata - one which never pops any elements from the stack. When the pushdown automaton reaches end of input (and ideally finds itself in an accepting state) it will contain all the output stored in its stack. 

![pushdown_and_gate_accepting](/assets/pushdown_and_gate_accepting.png)

One may see an interesting observation. In the previous section, I showed how multitatpe automata correspond to the philosophy of "computation through output recognition". In case of pushdown automata, the philosophy of the approach gets much closer to "computation through output generation".

Recall that formal definition of pushdown automaton is a 6-tuple _(Q,&Sigma;,&Gamma;,&delta;,q<sub>0</sub>,F)_ such that:

- _Q_ is the set of states,
- _&Sigma;_ is the input alphabet
- _&Gamma;_ is the stack alphabet
- _&delta; : Q &times; (&Sigma; &cup; {&epsilon;}) &times; (&Gamma; &cup; {&epsilon;}) &rarr; 2<sup>Q &times; (&Gamma; &cup; {&epsilon;})</sup>_ is the transition function. A transition fo the form _&delta;(q,&epsilon;,&gamma;)_ transitions nondeterministically without reading any input from &Gamma;. A transition of the form _&delta;(q,&sigma;,&epsilon;)_ reads input _&sigma;_ without reading anything from the stack. 
- _q<sub>0</sub>_ is the initial state
- _F &sub; Q_ is the set of accepting states

Notice that if we forbit the automaton from reading from the tape, the only allowed transitions then become _&delta; : Q &times; (&Sigma; &cup; {&epsilon;}) &times; {&epsilon;} &rarr; 2<sup>Q &times; (&Gamma; &cup; {&epsilon;})</sup>_. Since  _{&epsilon;}_ is a singleton set, we might as well just write _&delta; : Q &times; (&Sigma; &cup; {&epsilon;}) &rarr; 2<sup>Q &times; (&Gamma; &cup; {&epsilon;})</sup>_ and the _&epsilon;_ becomes always implied. But look! This transition function looks just like that of nondeterministic Mealy machine. 

Sometimes, for convenience the transition function of pushdown automata is defined as _&delta; : Q &times; (&Sigma; &cup; {&epsilon;}) &times; (&Gamma; &cup; {&epsilon;}) &rarr; 2<sup>Q &times; &Gamma;*</sup>_. This is analogical to [Mealy machines with string output](/transducers_intuitively.html#mealy-and-moore-machines-with-string-output)!

### Recognizing #-separeted pairs of strings

The pushdown approach to Mealy machines requires us to consider contents of stack as output of machine. This is not how pushdown automata were originally defined to work. We could instead encode two strings in one and then use pushdown automaton to recognize it, without the need to bend or abuse any original mathematical definitions.

For instance suppose that a Mealy machine works with alphabet _&Sigma;={0,1}_ and produces output _011_ for input _111_. We can add a new special symbol _#_ to alphabet _&Sigma;'={0,1,#}_ and then encode input-output pairs as _input#reversed\_output_. In this case it would be _111#110_. Next we can easily transform our Mealy machine to a pushdown automaton which recognizes such strings. Below is an example of how such conversion might work:

![pushdown_and_gate_accepting_pairs](/assets/pushdown_and_gate_accepting_pairs.png)

The idea is to add a transition over # coming out of every (previously) accepting state and connect it to a new state. This new state then becomes the new accepting state and it's the only  accepting state of the entire automaton. Its sole purpose is to accept the string that is stored on the stack. Also remember that a pushdown automaton accepts only when it reaches accepting state **and** has empty stack.


### Recognizing interleaved pairs of strings

There is one more encoding of pairs. Suppose you have a pair _(0010,abba) &isin; &Sigma;* &times; &Gamma;*_. You can zip those two strings into one _0a0b1b0a &isin; (&Sigma; &cup; &Gamma;)*_. The downside of this appraoch is that it only works for strings of equal lengths. However, if the two alphabets are disjoint _&Sigma; &cap; &Gamma; = &empty;_ then the pair _(0100101,abba)_ can be encoded as _0100101abba_, _010010abba1_, _0a10b0b1a01_ or _0ab10ba0101_ and so on, without leading to ambiguity. We can always easily decode _0ab10ba0101_ back to _(0100101,abba)_ by checking, which alphabet a given symbol belongs to. The only problem is that the encoding is ambiguous. The pair _(0100101,abba)_ can be encoded in hundreds of different but equivalent ways. For this reason we need to come of with the following definition of interleaved formal language:

> Let &Omega; be some finite set (alphabet) and let &Sigma; &cup; &Gamma; be a partition of &Omega;. A formal language over L &sub; &Omega;* is equivalent to relation of formal languages R &sub; &Sigma;* &times; &Gamma;* such that:  
> (&sigma;,&gamma;) &isin; R if and only if at least one of the interleaved strings encoding (&sigma;,&gamma;) is in L.  
> We will call L the interleaved language of R, whereas R is the relation induced by &Sigma; and &Gamma; on L.

Every Mealy machine can be converted to a finite state automaton accepting the corresponding interleaved language. You can do it be replacing evey Mealy transition:

![mealy_sigma_gamma_transition](/assets/mealy_sigma_gamma_transition.png)

with two FSA transitions:

![mealy_sigma_then_gamma_transition](/assets/mealy_sigma_then_gamma_transition.png)

where &sigma; &isin; &Sigma;&cup;{&epsilon;} and &gamma; &isin; &Gamma;&cup;{&epsilon;}.

Every finite state automaton (with partition &Sigma; and &Gamma; of its original alphabet) can be converted to a nondeterministic Mealy machine with &epsilon;-transitions that recognizes the relation induced by &Sigma; and &Gamma;. It can be done by replacing every transition over &sigma; &isin; &Sigma;

![mealy_sigma_transition](/assets/mealy_sigma_transition.png)

with 

![mealy_sigma_epsilon_transition](/assets/mealy_sigma_epsilon_transition.png)

and every transition over &gamma; &isin; &Gamma; 

![mealy_gamma_transition](/assets/mealy_gamma_transition.png)

should be replaced with

![mealy_gamma_epsilon_transition](/assets/mealy_gamma_epsilon_transition.png)

Lastly, it's worth pointing out that the interleaved languages have a lot in common with [normalization of Mealy machines](/mealy_normalization.html). 

