---
layout: post
title:  "Local languages and multitape automata"
date:   2020-05-30 01:07:00 +0200
categories: automata
permalink: /local_languages.html
---
**Contents**
* TOC
{:toc}
  
### Definition

Local languages are a very special kind of regular languages. 
Their defining property is that every such language can be secribed with 3 sets: _B&sub;&Sigma;_ which contains all the initial symbols, _E&sub;&Sigma;_ with all the ending symbols and _L&sub;&Sigma;&Sigma;_ with all the pairs of allowed consecutive symbols. For example

_&Sigma;={0,1,2,3,...9}_  
_B={0,1}_  
_E={4,5}_  
_L={00, 01, 12, 29, 25, 34, 44, 45, 50, 60, 70, 81, 90}_  
Then in order to check whether string _s=012900125_ belongs to the language we only need to check 3 things:

1. Whether _s_ begins with symbol from _B_. Here _0_ is in _B_ so it passes the check.
2. Whether _s_ ends with symbol from _E_. Here _5_ is in _E_ so it passes the check.
3. Whether every 2-factor belongs to _L_. A 2-factor is a substring of length 2. Here the consecutive 2-factors are: _01_, _12_, _29_, _90_, _00_, _01_, _12_, _25_. All of them are in _L_ so the check passes.

Every string accepted by a local language has then the form _(B&Sigma;* &cap; &Sigma;*E) \\ &Sigma;*(&Sigma;&Sigma;\\L)&Sigma;*_. You can see that _s &isin; (B&Sigma;* &cap; &Sigma;*E) \\ &Sigma;*(&Sigma;&Sigma;\\L)&Sigma;*_.

### Automata

The automata that accept local languages have a special property. Every state of the automaton (except the initial one) can be associated with some symbol of the alphabet &Sigma;. Therefore set of states _Q_ becomes _Q=&Sigma;&cup;{&epsilon;}_ where the &epsilon; is reserved for the initial state (however, not all automata require it. In some cases, the initial state &epsilon; might be equal to some state from &Sigma;). Moreover, for every symbol _x_ from _&Sigma;_, every transition with label _x&isin;&Sigma;_ must go to the respective state _x&isin;Q_. In other words every transition looks like this:

![local_transition](/assets/local_transition.png)

Here I give a full example of automaton that corresponds to the sample sets _B_, _E_, _L_ from above.

![local_automaton_example](/assets/local_automaton_example.png)

As you can see this automaton lacks some transitions, therefore it is not deterministic. However, it also doesn't contain &epsilon; transitions or nondeterministic ambiguous transitions. Hence it also isn't a truly nondeterministic automaton. Automata like this are called partial. Is short a partial automaton looks just like deterministic automaton, but sometimes some transitions might be missing.

### Multitape automata

Consider a finite state automaton which reads input from 2 strings at once. All of its transitions now are labeled with pairs of symbols instead of just one symbol. Below is an example of (partial) multitape automaton which reads two tapes, such that the first one has symbols from alphabet _{a,b}_ and the second tape is over alpphabet _{0,1}_.

![multitape](/assets/multitape.png)


In general case there might be even more inputs than two. Every additional input tape requires us to add more symbols on each transition. 

#### Encoding with product of alphabets 

Multitape input doesn't really increase the power of automata. We could very easily encode both of the input string in just one string. 

For instance suppose that first string is some text that uses letters from a to z and the second string is some stream of bits (ones and zeroes). You could then build a new alphabet consisting of lowercase letters a-z (for encoding letter and bit 0) and uppercase letters A-Z (for encoding letter and bit 1). For example let _s<sub>1</sub>=ajerfkn &isin; {a,...,z}*_ and _s<sub>2</sub>=0100110 &isin; {0,1}*_, then we can "merge" both of these strings into one new string
_s<sub>3</sub>=aJerFKn &isin; {a,...,z,A,...,Z}*_. You can in fact treat the new alphabet as Cartesian product of the two prevoius ones. In this case _{a,...,z}&times;{0,1}={(a,0),...(z,0),(a,1),... (z,1)}&#8776;{a,...,z,A,...,Z}_.

I could also give you an example of how a multitape automaton would look after encoding it with such alphabet:

![multitape_product](/assets/multitape_product.png)

In the general case, every time we want to encode input from multiple strings over two different alphabets _&Sigma;<sub>1</sub>_ and _&Sigma;<sub>2</sub>_ we can perform their Cartesian product _&Sigma;<sub>1</sub>&times;&Sigma;<sub>2</sub>_ to get merged alphabet. At this point you should see a very important limitation. Every multitape automaton reads the input symbols from both tapes at once, hence the inputs are always of equal lengths. Merging strings such as _s<sub>1</sub>=ajer &isin; {a,...,z}*_ and _s<sub>2</sub>=0100110 &isin; {0,1}*_ is not allowed, because one is shorter than the other!

#### Encoding with interleaved strings 

There is an alternative way of merging strings. Instead of performing Cartesian product of alphabets, we might equivalently just perform union of alphabets and then "zip" the two input strings character after character. 

For instance consider again the case of _s<sub>1</sub>=ajerfkn &isin; {a,...,z}*_ and _s<sub>2</sub>=0100110 &isin; {0,1}*_. We can zip the strings together to obtain _s<sub>3</sub>=0a1j0e0r1f1k0n &isin; ({a,...,z} &cup; {0,1})*_.

An automaton that recognizes such interleaved strings might looks like this:

![multitape_interleaved](/assets/multitape_interleaved.png)

Such automata have a few very special properties. One of them is that their graph is [bipartite](https://en.wikipedia.org/wiki/Bipartite_graph). We can rearrange the graph above to look like this:

![multitape_bipartite](/assets/multitape_bipartite.png)

You can see that all the arrows going down have labels from one alphabet (here it's _{0,1}_) and all the arrows going up are from the other (here it is _{a,b}_). This notion can be even generalised to more than two alphabets. For 3 tapes (and 3 alphabets), the graph can be divided into 3 partitions such that all edges outgoing from one partition, always go to vertices of another partition. It works analogically for even more tapes.


### Interleaved alphabets and local languages

There is a interesting 

First let's encode all the tapes into one interleaved string and annotate every symbol with tape. For instance given inputs _s<sub>1</sub>=ajerfkn &isin; &Sigma;\* = {a,...,z}\*_ and _s<sub>2</sub>=0100110 &isin; &Gamma;\* = {0,1}\*_ we can encode them as annptated string _s<sub>3</sub>=0<sup>&Gamma;</sup>a<sup>&Sigma;</sup>1<sup>&Gamma;</sup>j<sup>&Sigma;</sup>0<sup>&Gamma;</sup>e<sup>&Sigma;</sup>0<sup>&Gamma;</sup>r<sup>&Sigma;</sup>1<sup>&Gamma;</sup>f<sup>&Sigma;</sup>1<sup>&Gamma;</sup>k<sup>&Sigma;</sup>0<sup>&Gamma;</sup>n<sup>&Sigma;</sup>_. You may see that the annotations form a local language if you arrange them into a strings. In this particular example such a string of annotations is _&Gamma;&Sigma;&Gamma;&Sigma;&Gamma;&Sigma;&Gamma;&Sigma;&Gamma;&Sigma;&Gamma;&Sigma;&Gamma;&Sigma;_ and it belongs to a local language defined with: beginning symbols _B={&Gamma;}_, ending symbols _E={&Sigma;}_ and 2-factors _L={&Gamma;&Sigma;, &Sigma;&Gamma;}_. Therefore we can conclude that local languages are a good tool for describing dependencies between interleaved alphabets.

If you look at the bipartite graph from previous section above, you may see that a local automaton is "hidden" there. You can find it by merging all the states _q<sub>0</sub>_, _q<sub>1</sub>_, _q<sub>2</sub>_, _q<sub>3</sub>_ into first state of local automaton. Similarly all the remaining states _q0<sub>0</sub>_, _q0<sub>1</sub>_, _q0<sub>2</sub>_, _q0<sub>3</sub>_, _q1<sub>0</sub>_, _q1<sub>1</sub>_, _q1<sub>2</sub>_, _q1<sub>3</sub>_ would be merged together to form second state of local automaton. We can associate those states with alphabets. This would give us 

![multitape_local_quotient](/assets/multitape_local_quotient.png)

(Quick note for those readers who know abstract algebra: What we just did was finding a homomorphism between states and tapes. The kernel of such homomorphism yields a normal subgroup and the local automaton is isomorphic to the quotient group.)

If we had 3 tapes/alphabets, let's say _&Sigma;_, _&Gamma;_ and _&Omega;_, then the automaton could look like this:

![multitape_local_quotient2](/assets/multitape_local_quotient2.png)

You might wonder what's the practical application of this? Well, suppose you are given the annotated string _s<sub>4</sub>=0<sup>&Gamma;</sup>a<sup>&Sigma;</sup>1<sup>&Gamma;</sup>b<sup>&Sigma;</sup>0<sup>&Gamma;</sup>a<sup>&Sigma;</sup>0<sup>&Gamma;</sup>a<sup>&Sigma;</sup>1<sup>&Gamma;</sup>b<sup>&Sigma;</sup>1<sup>&Gamma;</sup>b<sup>&Sigma;</sup>0<sup>&Gamma;</sup>b<sup>&Sigma;</sup>_. You could strip it of the annotations and just feed the string 0a1b0a0a1b1b0b directly to the automaton

![multitape_interleaved](/assets/multitape_interleaved.png)

However, if there is a mistake somewhere in the string the automaton would fail without giving us any warning about it. (For instance the string _0a10ba0a1b1b0b_ would be rejected due swapped order of _0_ and _b_.) Having the annotations, we can first build the string _&Gamma;&Sigma;&Gamma;&Sigma;&Gamma;&Sigma;&Gamma;&Sigma;&Gamma;&Sigma;&Gamma;&Sigma;&Gamma;&Sigma;_ and feed it to

![multitape_local_quotient3](/assets/multitape_local_quotient3.png)

in order to validate its correctness. This way we can detect errors early. The best part of all this is that such approach can be extended to validate correctness of annotated regular expressions, before even compiling them to automata! With this can bring the power of multitape machines to regexes. This mechanism is the analogy of typecheckers used by Turing-complete programming languages. 


### Reading input strings of different lengths

So far we've been reading only one symbol from each tape at the time. This restricted the input only to cases when all input strings are of equal lengths. However, it doen't have to be this way. We can take one step further and generalise multitape automata in such a way that the length of input read from each tape might be different.

#### Multitape &epsilon;-transitions 

The key to reading tapes with strings of different lengths is nondeterminism, or more precisely, the &epsilon;-transitions. For example here is an automaton that reads two tapes but sometimes one of the tapes can "read" empty string &epsilon; (so tape's head holds still in place), while the other tape reads a specific symbol (tape's head advances by one cell):

![multitape_local_arbitrary_eps](/assets/multitape_local_arbitrary_eps.png)

This automaton could accept pair of strings _(10,1100000)_.

The only problem of such automata is that they can no longer be easily reduced to single-tape automata. If the alphabets are disjoint, we can treat one alphabet to be input, and the other to be output. This way the automaton becomes equivalent to Mealy machine. This is something that deserves [its own post](/recognizing_language_relations.html).

#### Interleaved strings of arbitrary size

There is however, and alternative approach that I want to cover here.
Let's say that we introduce two types of tapes. From the first type the automaton always reads only one symbol. From the second type, the automaton should read all consecutive symbols until it hits the special # symbol.  

For instance suppose there are two tapes: _&Sigma;_ with symbols _{0,1}_ and _&Gamma;_ with symbols _{0,1,#}_. The _&Gamma;_ tape contains the special # symbol, which means that the automaton should read input of arbitrary size from it. The # works as separator that tells us when to stop reading. The alphabet _&Sigma;_, on the other hand, doesn't contain #, so automaton can read symbols from it only one by one. Below is an example of automaton reading from _&Sigma;_ and _&Gamma;_.

![multitape_local_arbitrary](/assets/multitape_local_arbitrary.png)

This automaton can accept pairs of strings such as _(10,11#0#)_, _(10,11#00#)_, _(10,11#000#)_, ... _(1010,11#0#11#0#)_, _(1010,11#00#11#0#)_, _(1010,11#000#11#00#)_, ... and so on. Notice that there are always exactly as many # on the right, as there symbols in total on the left. Indeed you should treat the entire substring of the form _01010...1010#_ as one atomic query to tape. This way we still perform equal number of queries to each tape but the tapes of first tape return one symbol per query, while tapes of second type return the whole sequence _...#_ per query.

We could now easily merge both inputs into one interleaved strings. This time instead of "zipping" symbols, we "zip" queries. Hence  for example _(<ins>10</ins>,11#0#)_ becomes _<ins>1</ins>11#<ins>0</ins>0#_ (I underlined symbols from first string to make it easier to read and understand). 

Such automata can again be described using local languages. In the example above _&Gamma;_ can come after _&Sigma;_ therefore _&Sigma;&Gamma; &isin; L_. Moreover _&Gamma;_ can repeat, which means that _&Gamma;&Gamma; &isin; L_. Eventually we have to find _#<sup>&Gamma;</sup>_ at the end of such repetition and _#<sup>&Gamma;</sup>_ is a special symbol that deserves to be treatet as separate state of local automaton, therefore _&Gamma;#<sup>&Gamma;</sup> &isin; L_. After # the cycle repeats and we can read _&Sigma;_, so we put _#<sup>&Gamma;</sup>&Sigma; &isin; L_. Lastly the beginning symbol is _&Sigma;&isin;B_ (this is not the same as initial state!) and ending symbol  _&Sigma;&isin;E_. This yields automaton:

![multitape_local_interleaved_arbitrary](/assets/multitape_local_interleaved_arbitrary.png)

This will validate that any input string has the correct form _(&Sigma;&Gamma;\*#)\*_. Such mechanism can be further generalised to more than two tapes.