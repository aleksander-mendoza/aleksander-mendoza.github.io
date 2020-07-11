---
layout: post
title:  "Introduction to logic"
date:   2020-06-21 01:08:00 +0200
categories: logic
permalink: /intro_to_logic.html
---
**Contents**
* TOC
{:toc}

WORK IN PROGRESS

The purpose of this post is to present many different branches of logic in a concise and intuitive way.

#### Classical logic

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


#### Modal logic

However, things are not always so simple. What if Jake like dogs, but you don't know whether he likes cats too. Then we are using modal logic and we might express it as _&#9633;x&and;&#9671;y_ which stands for "x is necessary and y is possible". 

#### Temporal logic

It might also be the case that Jakes used to like dogs when he was young, but as he got older he changed his mind. In such scenario we would use temporal logic. We could express such statement as _x&and;**X**&not;x_, which translates to "Jakes likes dog (x) at one point in time (when he's young) and (&and;) at the next (**X**) point in time (when he's older) he does not (&not;) like them (x)".

#### Fuzzy logic

What if Jake isn't completely sure whether he likes the dogs or not. He would rather say that he likes dogs only a little but he likes cats much more. We should express such scenario using fuzzy logic with truth values between 0 and 1 (so 0 is absolute falsity, 1 is absolute truth and 0.5 is somewhere in between). For example _x=0.12_ and _y=0.75_ then _x&and;y=min(x,y)=0.12_. 

#### Probabilistic logic

Imagine that someone asks us whether Jake likes dogs and we are not entirely sure. Maybe we heard Jake make nice comments about dogs 75% of the time and only 25% of the time he would say something bad about them. Then we could use probabilistic logic and take a guess that, next time we ask Jake the question "Do you like dogs?" he will say "yes" with 75% probability (_x=1_ around 75% of the time)

#### First order logic 

It might also be the case that Jake likes some dogs but not all of them. Then we should imagine that there is a set _D_ containing all the dogs in the world and _x_ is no longer just true or false. Instead _x_ now depends on the exact dog from _D_, which means that we need to use predicate logic. Now we can define a predicate _X(d)_="Jake likes dog _d_ ". We could then use quantifiers _&forall;<sub>d</sub>X(d)_ meaning "Jake likes all dogs" and _&exist;<sub>d</sub>X(d)_ meaning "There exists at least one dog that Jake likes". We might notice that _x_ is the same as _&forall;<sub>d</sub>X(d)_.

#### Modal vs. First order logic

There is a very interesting observation to be made. Think once again about modal logic and the expression _&#9633;x&and;&#9671;y_. The quantifier _&#9633;_ stands for "it is necessary that", however, we could see it in a more metaphysical way. Think of all the possible universes that could ever exist and in all of them Jake might be a bit different. Maybe there is a universe in which Jake likes dogs and there is some other universe in which he doesn't. Now put all the universes in a set _U_ and build a predicate _X(u)_="Jake from universe _u_ likes dogs". Do the same for _Y(u)_ and voila! We can express _&#9633;x&and;&#9671;y_ as _&forall;<sub>u</sub>X(u)&and;&exist;<sub>u</sub>Y(u)_. 

#### Linear logic

Classical logic allows us to make statements such as "if _x_ then _x_ and _x_". It sounds to be true in principle. For instance "if Jake likes dogs, then Jake likes dogs and also he likes dogs too". However, sometimes we might want to consider statements that operate on some kind of consumable resources. For instance, imagine that one dog costs 100\$. If Jake has 100\$ then he can buy 1 dog. It's no longer true that "If Jake has 100\$ then he can buy 1 dog and he can buy 1 dog", because that would give him a total of two dogs. Linear logic was created to better describe such reasoning. 

It has found great applications in computer science, for dealing with allocable memory (see for instance programming languages such as Rust, ATS or Idris2).

#### Vacuous arguments

Could you imagine a universe in which Jake both likes dogs and hates dogs at the same time? And, no, I'm not talking about universes in which Jake has schizophrenia or multiple personality disorder. There is a much simpler explentation for such anomalous behaviour. Imagine a universe in which Jake doesn't exist. If there is no Jake, then I might say that he likes dogs. Who's to say that I'm wrong? I might also say that he doesn't like them at the same time. If there is no Jake, then there is no way to decide. 

#### Quantum logic

The example of of nonexistent Jake kind of resembles the Schrödinger's cat, which is both dead and alive at the same time. The primary difference, however, is that missing Jake represents logic which is vacuously true, whereas Schrödinger's cat represents a case for quantum logic. Unfortunately I don't know enough about quantum mechanics, to tell you much more about it.

#### Intuitionistic logic

Think what it means to _know_ that "Jake likes dogs". Have you met Jake? Did he tell you about it? Maybe he lied. Or maybe some stranger on the internet told you about Jake and made everything up? To be really sure, you should ask for evidence. It would be great if we found some witness. However, even if we find someone claiming to witness Jakes love for dogs, we can't really be sure that the witness is lying. Therefore we need a "non-human" witness. For instance, if we could find some document saying that Jake is a volunteer at a local dog shelter, that would be a definitive proof and the document itself would be a witness. This leads us to the conclusion that in this world, nothing is inherently true or false. Instead some things have been witnessed to be true, some other things have been witnessed to be false and a lot of other things have not been witnessed yet at all. This philosophy corresponds to intuitionistic logic point of view. 


#### Paraconsistent logic

Christianity describes god as omnipotent entity, which means that he can do anything he wants. Does it mean that god could create a rock so heavy that even he himself cannot lift it? If he can create it, then he cannot lift it, which means that he is not omnipotent. If he cannot create such rock then he also isn't omnipotent. This leads to contradition. In classical logic contradictions are not allowed and any scenario that leads to contradition is considered impossible. This means that we have just proved, that no such thing as omnipotence can exist. However, some logicians decided to abolish the idea that everything is either true or false. This lead to creation of paraconsistent logic, which allows for some things to be both true and false at the same time. 


#### Infinitary logic

In classical logic one can use &not;, &and;, &or; and &rarr; to connect variables together and build formulas such as _x &and; y &rarr; x &or; y_. Usually you can easily manipulate such formulas on sheet of paper or in a computer. Infinitary logic makes things much harder and more complex by allowing infinitely long formulas. For instance, the number &pi;=3.141592653589793... has the infinitary nature, as it could never be written down in its entirety. Similarly infinitary logic admits formulas like _x<sub>1</sub> &and; x<sub>2</sub> &and; x<sub>3</sub> &and; x<sub>4</sub> &and;... x<sub>&infin;</sub>_. Notice the striking similarity between this and the quantifier _&forall; x_. Analogically for _x<sub>1</sub> &or; x<sub>2</sub> &or; ... x<sub>&infin;</sub>_ and _&exist; x_.


#### Computability

Logic and computers have a lot in common. Indeed all logical formulas can be encoded with bits. Classical truth tables can be computed. Deductive proofs can be automatically validated. Intuitionistic reasoning can be even encoded in the fabric of algorithms themselves (this is called Curry-Howard isomorphism). Computation is what gives the meaning to all of logic. In fact, when we humans try to think logically, our reasoning becomes more and more similar to that of execution of computer programs. If you learn about computability theory you will notice a lot more.

#### Uncomputability

Similarly to case of omnipotent god, imagine a supercomputer of divine power that can look at any other computer and tell how fast it is. In particular, the supercomputer can tell much time it will take for any other computer to finish a specific task. Also notice that some tasks require infinite amount for time, like for example this infinite loop:

> step 1: go to step 2  
> step 2: go to step 1  
> step 3: finish  

Suppose that we rigged one of such supercomputers to malfunction and now it will enter infinite loop, whenever it checks some machine that finishes in finite time:

> check time of computer X on task Y  
> if X finishes task Y in less that &infin; time then loop infinitely

What happens if we ask the rigged supercomputer to check itself (and the task is to check whether it will loop forever)? This argument is very similarto that of heavy rock and omnipotent god. The rigged computer is like a heavy rock. The divine supercomputer is like omnipotence. It leads to contradiction and proves that no computer can tell whether other computers loop ofrever or not. This uncomputable problem is called _the halting problem_.

##### Philosophical implications of uncomputability

This has certain philosophical implications, because it tells us that some problems are impossible to solve. Moreover, halting problem was used by Kurt Gödel to show that arithmetic could not be axiomatized. This poses heavy limits on what can be achieved by logical reasoning.  Gödel's proof shattered optimism of many mathematicians, who were searching for definite foundation for all of mathematics. As it turns out, there are no "fundamental laws" from which all of mathematics could be derived. This problem also carries further to metaphysics - perhaps the laws of physics have no end and humanity will forever discover more and more. First we discovered atoms, then we found out that there are even smaller particles such as quarks. Now physicists are trying to investigate deeper and deeper into quantum world. Maybe this cycle has no end, and there will always be a smaller scale and new physics to discover.  

##### Infinitary objects

There are more uncomputable things in the world than one might expect. The infinitary logic, has a lot in common with &omega;-languages, which specifically focus on machines that compute in infinite time. Real numbers are inherently uncomputable, and since many values in real world seem to be based on real values (positions and speed of particles, intensity of forces, energy levels etc.) it means that our universe is inherently uncomputable. There is, however, a special kind of machines, called _Turing machines with Oracle_, which are capable of "magically" performing uncomputable tasks. There exists a concept of _Turing jumps_, which informally says that there are infinitely many difficulty levels of uncomputable tasks. If our universe was a simulation, then we are for sure a product of Turing machines with Oracle. And if we are simulated in a universe, where uncomputable tasks are computable, then the next "Turing step" would be the existence of even more powerful universe. Each higher universe would allow for building machines capable of computing all the uncomputable tasks of lower universes. 

#### Complexity

There exist many sorting algorithms. You might even know some of them. Bubble sort, insertion sort, quick sort, merge sort. There is however, one very special and intersting algorithm called random sort (or Bogosort). The idea is simple. Your input is a list of numbers, let's say 1,5,8,4. You try to shuffle this list randomly and see if the obtained result is sorted. Repeat until evetually you get it right. By itself, this approach might look like a very inefficient sorting algorithm. However, suppose that we live in a nondeterministic world (in fact, nobody knows for sure whether all of the physics is deterministic or not). Maybe there is some truly random phenomenon in quantum mechanics, whose outcome is indeed nondeterministic. Every time such phenomenon takes place, it would cause branching of our universe's timeline and each individual branch is equally possible and equally consistent with laws of physics. We can use this to our advantage and build a truly random machine, which computes by performing nondeterministic instructions. We could then tell it to use Bogosort and it would suddenly become the most efficient sorting algorithm ever created. The machine would use the nondeterministic events to split our universe's timeline. We could this way generate all possible list configurations [1,5,8,4], [5,4,1,8], [4,8,5,1]... and so on. One of the branches would successfully obtain list [1,4,5,8]. The only side-effect is that all the other timelines would be out of luck. (But who cares about them? It's important that in one timeline we got it right. Maybe there is a timeline in which Hitler won the war, but I don't see people being sad about it in our timeline. You get the idea.)

This is the essence of complexity theory, presented with philosophical flavour. Very often hard tasks become easy, when you can perform nondeterministic computation. 


#### Orthogonality and axioms

Every logician should be well aware that certain statements are logically independent. For instance, if I told you that Jake likes dogs, that doesn't tell you anything about whether he likes cats or not. In arithemtic, you might have two independent equations, like _x=y+3_ and _a=b*5_. If I tell you that _y=1_, then you can calculate _x=4_, but you know nothing about _a_ and _b_. In mathematics there are several famous example of such independent statements, such as the axiom of choice or the continuum hypothesis. In fact, any time when mathematics encounters a new orthogonal statement, people start to treat it like an axiom. An axiom is in essence, some logical statement that is just taken to be true, without asking for any proof of it. A logician, may or may not admit certain axioms. A set of admitted axioms along with all their logical consequences is called a <ins>theory</ins>. Indeed this is real definition of what a theory means (even though, in everyday language, this word has completely different meaning). It's worth pointing out that all admitted axioms are (or at least should be) always independent from each other, because if validity of one axiom could be prooved using all the other axioms, then admitting it becomes pointless. Similarly, you should never be able to proove any of the axioms to be false (unless you work with paraconsistent logic). All of the axioms should in principle neither be provable, nor disprovable. 

##### Metaphysical interpretation of orthogonality

Suppose for a moment that in the fabric of our reality, there are in fact hidden some very specific axioms, from which we could derive all laws of physics. Maybe those axioms govern most of the nature's phenomena, but not all. Maybe there are some "logical holes", that give rise to a certain degree of freedom in the outcome of various physical phenomena. Such degrees of freedom would give rise to nondeterminism, which by its very nature is a purely logical notion.  I already tackled nondeterminism in the section on "Complexity", but now you should be able to see it in more axiomatic way. Indeed, it directly stems from logical orthogonality. We could try to deal with nondeterminism by visualizing its effects as branching of timeline. Each possible nondeterministic outcome is orthogonal to previous state of universe and can neither be proved nor disproved. We can see that branching timeline would not be an axiom of physics, but rather lack thereof. 

#### Lattices

Things get even more interesting, when one logical statement follows from the other, but not vice versa. For instance, consider the difference between saying that "Jake likes animals" and "Jake likes dogs". Dogs are animals but animals are not dogs. Similarly, some axioms can be more general than other. One axiomatic system can contain all logical statements of another and extend it even further. This gives entire lattice of theories (let us recall that theory is defined as set of true formulas). For instance, suppose we have only two formulas &phi; and &psi; (in classical logic, those might be formulas like _x&and;y_ or _x&rarr;x&and;x_ ; in temporal logic they might look like _**X**&not;x_ or _y&rarr;**G****X**x_ ; etc), which gives us the following lattice: 

![implication_lattice_xy.svg](/assets/implication_lattice_psi_phi.svg)

The empty set {} represents a theory in which nothing holds (usually that would be a theory with no axioms at all). On the opposite side is {&phi;, &psi;} in which everything holds. Both {} and {&phi;, &psi;} are called trivial theories and all the other theories in between are called non-trivial. Notice that {} is the most general theory because it doesn't specify anything, then {&phi;} is a certain "specialization" of {}. Later {&phi;, &psi;} is more specific than both {&phi;} and {&psi;}. Below, you can see an example of lattice for 4 formulas: "x", "y", "&not;x" and "&not;y"


![implication_lattice_xy.svg](/assets/implication_lattice_xy.svg)

Notice how {x,&not;x} is an inconsistent non-trivial theory (this is something you might find in paraconsistent logic), while the theories {x,y}, {&not;x,y}, {x,&not;y}, {&not;x,&not;y} are called complete (each formula is either true or false but not both and not none).

Lattices give rise to semantics behind every logic! In fact the logical connectives of &or; and &and; correspond to "join" and "meet" operations on lattices. Boolean algebra and Heyting algebra study them in depth. 






#### Intuitionistic vs. modal logic

In intuitionistic logic, there is the notion of things that are yet waiting to be proven. It resembles temporal logic,

#### Intuitionistic vs. classical logic

In classical logic we could use truth tables and evaluate every logical formula to either 1 or 0. In intuitionistic logic, it's impossible because there are infinitely many "truth" values. The bottom value &perp; is used for representing something that could never be witnesed or proved. The top value &#8868; represents an "obvious" fact that is always observed to be true. Then there could be an infinite number of intermediate values in between, each one corresponding to a different proof. For example in this graph every node corresponds to one value

![heyting_p_not_p](/assets/heyting_p_not_p.png)




