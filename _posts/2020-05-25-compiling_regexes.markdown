---
layout: post
title:  "Compiling regexes to automata"
date:   2020-05-25 01:06:00 +0200
categories: automata
permalink: /compiling_regexes.html
---

### Backtracking

There are several ways to run regexes. The main two approaches are with backtracting and by compiling to automata. Most (all?) programming languages that have support for regexes don't actually use automata at all, but instead they use backtracking. Here is a simplified implementation of such algorithm:

{% highlight C++%}

interface Regex{
    /** Returns number of matched characters. If that number 
    equals length(input) then the whole input was matched. 
    When whole input is matched that means it was accepted 
    by the regular expression. */
    int backtrack(String input);
}

class Union implements Regex{
    Regex lhs,rhs;
    int backtrack(String input){
        return max(lhs.backtrack(input),rhs.backtrack());
    }
}
class Concatenation implements Regex{
    Regex lhs,rhs;
    boolean backtrack(String input){
        int lhsMatch = lhs.backtrack(input);
        return lhsMatch + rhs.backtrack(input[lhsMatch...]);
    }
}
class KleeneClosure implements Regex{
    Regex child;
    boolean backtrack(String input){
        int offset = 0;
        while(offset<length(input)){
            int furtherOffset = child.backtrack(input[offset...]);
            if(furtherOffset==0)break;
            offset += furtherOffset;
        }
        return offset;
    }
}
class Letter implements Regex{
    char letter;
    boolean backtrack(String input){
        if(firstChar(input)==letter)return 1;
        return 0;
    }
}

{% endhighlight C++%}

This code is not 100% working but it's enough to sketch the idea. For instance if you want to match regex `a(c|b)*` you need to first translate it into the following data structure

    Concatenation(Letter('a'), KleeneClosure(Union(Letter('c'),Letter('b'))))
    
Let's say that you want to run it on `input="abbc"`. The backtracking could be illustrated as follows:  
<table>
<thead>
  <tr>
    <th>step</th>
    <th>input read so far</th>
    <th>next char</th>
    <th>reached node</th>
    <th>status</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>1</td>
    <td>""</td>
    <td>"a"</td>
    <td>Concatenation Letter('a')</td>
    <td>ok</td>
  </tr>
  <tr>
    <td>2</td>
    <td>"a"</td>
    <td>"b"</td>
    <td>Concatenation KleeneClosure Union Letter('c')</td>  
    <td>backtrack</td>
  </tr>
  <tr>
    <td>3</td>
    <td>"a"</td>
    <td>"b"</td>
    <td>Concatenation KleeneClosure Union Letter('b')</td>  
    <td>ok</td>
  </tr>
  <tr>
    <td>4</td>
    <td>"ab"</td>
    <td>"b"</td>
    <td>Concatenation KleeneClosure Union Letter('c')</td>  
    <td>backtrack</td>
  </tr>
  <tr>
    <td>5</td>
    <td>"ab"</td>
    <td>"b"</td>
    <td>Concatenation KleeneClosure Union Letter('b')</td>  
    <td>ok</td>
  </tr>
  <tr>
    <td>5</td>
    <td>"abb"</td>
    <td>"c"</td>
    <td>Concatenation KleeneClosure Union Letter('c')</td>  
    <td>ok</td>
  </tr>
  <tr>
    <td>5</td>
    <td>"abbc"</td>
    <td>""</td>
    <td>Concatenation KleeneClosure</td>  
    <td>ok</td>
  </tr>
</tbody>
</table>

In other words, this greedy algorithm just tries all possible routes and attempts to match as far as it can. Each time it cannot match any further, it backtracks and tries a different route.
    
This approach is unfortunately very slow in general. There are even [stackoverflow questions](https://unix.stackexchange.com/questions/419545/why-is-regular-expression-matching-so-slow) concerning it. Compiling regexes to automata can be done very efficiently and once you obtain (deterministic) automaton matching can be done extremely fast. The only reason why we don't compile regexes to automata is because backtracking allows us to use look-aheads, look-behinds and capture groups, which would otherwise not be possible. There are many different ways to use those nice features. You can for example perform
`"some sample string".replaceAll("(?<= )s","z")` and obtain  `"some zample ztring"`. Or you can run `"some sample string".replaceAll("(.)s","s$1")` and get `"some s ample s tring"`. Well... actually most of the time when you use those feature you probably just want to replace something. This exact task can be easily performed with [Mealy/Moore machines](/transducers_intuitively.html) and it could run magnitudes faster. At the end of this article I will present ways in which special regular expressions with output could be constructed and compiled.


### Thompson's construction

By far the easiest to understand and often presented on computer science courses is the Thompson's construction. Here is a sketch of the idea behind it.

![Thompsons_construction](/assets/Thompsons_construction.jpg)

Let's see an example of how to perform it on the expression `a(c|b)*`. First you construct automaton that accept just the letter `c`.

![Thompsons_construction_c](/assets/Thompsons_construction_c.png)

Then you do the same for `b`.

![Thompsons_construction_b](/assets/Thompsons_construction_b.png)

Next you combine them together to obtain automaton accepting `c|b`.

![Thompsons_construction_c_or_b](/assets/Thompsons_construction_c_or_b.png)

Next you perform Kleene closure `(c|b)*`.

![Thompsons_construction_c_or_b_star](/assets/Thompsons_construction_c_or_b_star.png)

Lastly you build automaton for `a`

![Thompsons_construction_c](/assets/Thompsons_construction_a.png)

and concatenate it to obtain end result `a(c|b)*`

![Thompsons_construction_a_and_ c_or_b_star](/assets/Thompsons_construction_a_and_c_or_b_star.png)


The idea is based around keeping track of one initial state and one accepting state, then using them to glue different automata together like LEGO bricks. While very simple and intuitive, it has one major disadvantage. Those automata are bloated with &epsilon;-transitions and have unncessarily many states. Here is equivalent automaton but with only 2 states:


![Thompsons_construction_minimal](/assets/Thompsons_construction_minimal.png)

### Glushkov's construction


This algorithm addresses many problems of Thompson's construction construction. Here we will produce automata that have significantly less states and have no &epsilon;-transitions. The basic idea is to associate states of produced automaton with letters in regular expression. For example consider `a(c|b)*` again. We should create 4 states: one for each of the latters `a`, `b`, `c` and one extra state that will be initial (kind of as if that fourth state stood for &epsilon;). Let state A be for `a`, B for `b`, C for `c` and Q for &epsilon;

![Glushkovs_construction_states](/assets/Glushkovs_construction_states.png)

Now we need to add transitions. Let's start with `a`. Should it be possible to transition from `a` to `b`? Any string matched by this expression must begin with `a` and then the next one might be `b`. In other words, there is a string of the form `"...ab..."` that our expression accepts. You may observe that the exact same thing holds for `c`. Hence there should be transitions from A to B and from A to C.

![Glushkovs_construction_A](/assets/Glushkovs_construction_A.png)

Once we reach to letter `c`, the next letters that could be reached are `c` and `b` (that's due to Kleene closure). That means, there is a string of the form `"...cc..."` or `"...cb..."` that is accepted by our regular expression. So we obtain

![Glushkovs_construction_AC](/assets/Glushkovs_construction_AC.png)

A very similar reasoning hold for `b`. We can reach both `b` and `c` after `b`.

![Glushkovs_construction_ACB](/assets/Glushkovs_construction_ACB.png)

At this point we ran out of letter to check. There remains yet to look for letter that can be found at the beginning of strings. We can notice that strings of the form `"a..."` are accepted by the regular expression. Hence there should be some transition from Q (initial state) to A.

![Glushkovs_construction_ACBQ](/assets/Glushkovs_construction_ACBQ.png)

No string can begin with `b` or `c`, so there cannot be any transition from Q to B or C. The last thing that's missing are the accepting states. Let's see what letter can end our strings. There certainly are many strings of the form `"...c"` and `"...b"`. Hence C and B should be accepting.

![Glushkovs_construction_ACBQend](/assets/Glushkovs_construction_ACBQend.png)

This concludes the Glushkov's construction. Automaton is finished!

A very important thing needs to be observed. We would have a great problem if a certain letter appeared twice in the expression. For instance we could not compile this expression `ba(b|c)*`. In this case there is a way to reach `a` after `b`, but that does not mean that the string `"babab"` is accepted. Glushkov's construction only works for [local languages](/local_languages.html). What exactly it means is another topic. The important thing is that before compiling any expression this way, we must first localize it. 

Localization of a language can be performed by renaming every letter with a new one. For instance given expression `ba(b|c)*`, we shall renamed it as `wx(y|z)*`. Then we compile it to automaton:

![Glushkovs_construction_WXYZ](/assets/Glushkovs_construction_WXYZ.png)

and then we "undo" the renaming, by keeping track which letter corresponds to which. In this case w&rarr;b, x&rarr;z, y&rarr;b, z&rarr;c.

![Glushkovs_construction_BABC](/assets/Glushkovs_construction_BABC.png)

This way you can compile all regular expressions (not only local ones). Now with this knowledge you can move on and read about analogical [constructions for Mealy machines](/compiling_mealy_regexes.html)


