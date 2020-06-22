---
layout: post
title:  "Introduction to logic"
date:   2020-07-21 01:08:00 +0200
categories: logic
permalink: /intro_to_logic.html
---
**Contents**
* TOC
{:toc}

This blog post is going to be very different from most of the content that you could find on this site. It's meant to be an intersting and inspiring introduction to the beauty of logic. Essentially I want to give all readers a bone that will motivate them to further study. It's also the essence of logic that often inspires myself. 

Logic has its very roots in philosophy. Perhaps one of the fields most heavily reliant on logic is the study of epistemology. It asks questions such as "What do we know?", "What does it mean to know something?", "Can we ever be sure that what we know is actually true?", "What is truth?" and "Is there an ultimate truth in the universe?". Philosophers and mathematicians created the idea of classical logic, which assumes that there exists truth and falsity. If something is true then it's negation is false and vice versa. Everything is either true or false. There are notions of conjunction (logical and), disjunction (logical or) and implication, which can be used to build more complex logical expressions. For instance

> Jake likes dogs

might one logical sentence, which can be either true or false.

> Jake likes cats

is another sentence and they can be joined together to build

> Jake likes cats AND Jake likes dogs

Mathematicians would simplify it and introduce special notation. They would say that _x_ stands for liking dogs, _y_ stands for liking cats and _x &and; y_ stads for liking both dogs and cats. George Boole proposed a genious idea to denote truth with _1_ and falsity with _0_. This enabled mathematicians to treat all logic like numbers and make such fancy tables:

<table>
<thead></thead>
<tbody>
   <tr>
    <td>x&and;y</td>
    <td>x=0</td>
    <td>x=1</td>
  </tr>
  <tr>
    <td>y=0</td>
    <td>x&and;y=0</td>
    <td>x&and;y=0</td>
  </tr>
  <tr>
    <td>y=1</td>
    <td>x&and;y=0</td>
    <td>x&and;y=1</td>
  </tr>
</tbody>
</table> 


However, things are not always so simple. What if Jake like dogs, but you don't know whether he likes cats too. Then we are using modal logic and we might express it as _&#9633;x&and;&#9671;y_ which stands for "x is necessary and y is possible". 

It might also be the case that Jakes used to like dogs when he was young, but as he got older he changed his mind. In such scenario we would use temporal logic. We could express such statement as _x&and;**X**&not;x_, which translates to "Jakes likes dog (x) at one point in time (when he's young) and (&and;) at the next (**X**) point in time (when he's older) he does not (&not;) like them (x)".

What if Jake isn't completely sure whether he likes the dogs or not. He would rather say that he likes dogs only a little but he likes cats much more. We should express such scenario using fuzzy logic with truth values between 0 and 1 (so 0 is absolute falsity, 1 is absolute truth and 0.5 is somewhere in between). For example _x=0.12_ and _y=0.75_ then _x&and;y=min(x,y)=0.12_. 


Imagine that someone asks us whether Jake likes dogs and we are not entirely sure. Maybe we heard Jake make nice comments about dogs 75% of the time and only 25% of the time he would say something bad about them. Then we could use probabilistic logic and take a guess that, next time we ask Jake the question "Do you like dogs?" he will say "yes" with 75% probability (_x=1_ around 75% of the time)


It might also be the case that Jake likes some dogs but not all of them. Then we should imagine that there is a set _D_ containing all the dogs in the world and _x_ is no longer just true or false. Instead _x_ now depends on the exact dog from _D_, which means that we need to use predicate logic. Now we can define a predicate _X(d)_="Jake likes dog _d_ ". We could then use quantifiers _&forall;<sub>d</sub>X(d)_ meaning "Jake likes all dogs" and _&exist;<sub>d</sub>X(d)_ meaning "There exists at least one dog that Jake likes". We might notice that _x_ is the same as _&forall;<sub>d</sub>X(d)_.

There is a very interesting observation to be made. Think once again about modal logic and the expression _&#9633;x&and;&#9671;y_. The quantifier _&#9633;_ stands for "it is necessary that", however, we could see it in a more metaphysical way. Think of all the possible universes that could ever exist and in all of them Jake might be a bit different. Maybe there is a universe in which Jake likes dogs and there is some other universe in which he doesn't. Now put all the universes in a set _U_ and build a predicate _X(u)_="Jake from universe _u_ likes dogs". Do the same for _Y(u)_ and voila! We can express _&#9633;x&and;&#9671;y_ as _&forall;<sub>u</sub>X(u)&and;&exist;<sub>u</sub>Y(u)_. 

Could you imagine a universe in which Jake both likes dogs and hates dogs at the same time? And, no, I'm not talking about universes in which Jake has schizophrenia or multiple personality disorder. There is a much simpler explentation for such anomalous behaviour. Imagine a universe in which Jake doesn't exist. If there is no Jake, then I might say that he likes dogs. Who's to say that I'm wrong? I might also say that he doesn't like them at the same time. If there is no Jake, then there is no way to decide. 

The example of of nonexistent Jake kind of resembles the Schrödinger's cat, which is both dead and alive at the same time. The primary difference, however, is that missing Jake represents logic which is vacuously true, whereas Schrödinger's cat represents a case for quantum logic. Unfortunately I don't know enough about quantum mechanics, to tell you much more about it.

As you can see, there is a plethora of different logics. However, all the logics we've discussed so far had something in common. They all assume that every logical expression could take some truth value once we substitute all the unknown variables. This is broadly known Tarski's approach to logic. This mathematician was the father of model theory, which enabled mathematicians to study logical formulas as if they were functions (logical variables are inputs and some "truth" value is the output). There is, though, an alternative school of thought, coined by Heyting, which will require you to completely change the way you view logic. It is better known as intuitionistic logic.


Think what does it mean to _know_ that "Jake likes dogs". Have you met Jake? Did he tell you about it? Maybe he lied. Or maybe some stranger on the internet told you about Jake and made everything up? To be really sure, you should ask for evidence. It would be great if we found some witness. However, even if we find someone claiming to witness Jakes love for dogs, we can't really be sure that the witness is lying. Therefore we need a "non-human" witness. For instance, if we could find some document saying that Jake is a volunteer at a local dog shelter, that would be a definitive proof and the document itself would be a witness. This leads us to the conclusion that in this world, nothing is inherently true or false. Instead some things have been witnessed to be true, some other things have been witnessed to be false and a lot of other things have not been witnessed yet at all. 

Mathematics may be perhaps the most important example where Tarski's and Heyting's duality are at odds. This is closely related to the famous question asking, whether mathematics was discovered or invented. Probably the closest to the truth would be saying that theorems are discovered, but proofs are invented. In intuitionistic logic and especially in type theory, there is a clear distinction between proofs and theorems. Every theorem is a type. Every proof is the inhabitant (often called witness) of that type. Writing the following  
_a : A_  
means that _a_ is the proof of _A_. Essentially it's analogical to writing _a&isin;A_. In order to prove _A &and; B_ in type theory, we should first write is as cartesian product _A &times; B_. Then a pair _(a,b)_ is a proof of _A_ and _B_  
_(a,b) : A&times;B_   
In order to prove implication _A&rarr;B_, one should notice that _&rarr;_ used in logic means "if...then" but in rest of mathematics it denotes functions and it's not an accident. In type theory, proof on implication is a function. In particular proof of _A&rarr;B_ would be some procedure that starts with _a:A_ and after performing some operations, returns _b:B_ as a result.   
There is a special type &perp; called "bottom type" which has no inhabitants. It's analogical to set theoretic empty set &empty;. This is also they only way of expressing negation in intuitionistic logic. There is no such thing as "negative proof" that would be inhabitant of _&not;A_. Instead we write that _A&rarr;&perp;_, which means that if you could find an inhabitant of _A_ then you could apply it to function _A&rarr;&perp;_ and obtain inhabitant of &perp;, which is impossible, hence there could not be any proof of _A_.
If you think again about the example of universe in which Jake did not exist you could think of it as a function "Jake &rarr; Jake likes dogs" that takes a proof of Jake's existence and returns "".


Humanity has found many important laws and formulas that seem to perfectly describe our world. Calculus and differential equations govern the motions of planets, electricity, and flow of water current. Logistic map, chaos theory and fractals are hidden in nature all around us. Bayes' theorem, stochastic processes and normal probability distributions can describe all randomness and  entropy. 


Isaac Newton and many mathematicians after him had the optimistic view that if only someone knew the exact state of the universe at a particular point in time, then mathematics could be used to precisely calculate all of the past and future. Unfortunately in the subsequent centuries this idea has been disproved. Chaos theory showed examples of chaotic systems in which small variations at the beginning can cause enormous differences in the future (commonly known as butterfly effect). The only way to compute the later stages without commiting large errors, is if we knew the initial state precisely. This is not possible and will never be, because many quantities in the universe require use of real numbers and those numbers have infinite representations. Neither our brains, nor even best computers could ever work with infinite quantities. What's worse is that real numbers and measurement precisions is not the only problem. Kurt Gödel showed that some areas of mathematics could never be described with finite number of laws and formulas (see Gödel's incompleteness theorem for details). Moreover, computability theory showed us countless examples of problems that are inherently unsolvable by any means. Complexity theory went even further identified many key problems that, despite being theoretically solvable, are so hard that anybody attempting to solve them would have to wait for the results longer than the lifetime of universe itself (this is the principle on which modern cryptography is founded).




