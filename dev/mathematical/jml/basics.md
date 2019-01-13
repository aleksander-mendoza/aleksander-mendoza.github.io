#Basics

###What is JML?

JML is short for java modeling language. It's a tool for automating formal verification of java code. The main idea behind such proofs is as following:

1. First create formal specification of your algorithms
2. Then translate it into mathematical theorems and proofs
3. And finally run some verifiers to actually analyze if our logic is valid

If you want to dive deeper into formal proofs and automated theorem verifiers, you should learn some [Coq](https://coq.inria.fr), [Isabelle](https://isabelle.in.tum.de), [Idris](https://www.idris-lang.org) or type theory. However, in this tutorial we will focus more on using verifiers in practice with [OpenJML](https://www.openjml.org) and Java. If you don't like Java, you could still follow along with C and [Frama-C](https://frama-c.com). It's largely similar with some C-specific quirk but if you know one, then learning the other should be piece of cake. Alternatively if you for some reasong don't like OpenJML, you could also  try out [Krakatoa+Jessie](http://krakatoa.lri.fr) but this tutorial won't cover them.

**Note:** JML is a very niche tool and there are extrememly few tutorials for it. Here I will focus on making a user-friendly how-to guide with lot's of examples. However, it is strongly adviced to go through [official manual](http://www.eecs.ucf.edu/~leavens/JML/jmlrefman/jmlrefman_2.html) as well.

###How it works

JML is written entirely in form of Java comments. 

    //This is standard java comment
    /*And this is 
    block comment*/
    
To turn them into JML you just need to add @

    //@JML stuff
    
    /*@
    JML stuff
    @*/
    
    /*
    @NOT valid JML
    @NOT valid JML
    */
    
    /*
    @NOT valid JML
    @NOT valid JML
    */
    
So for single line JML use `//@` and for multiline use `/*@ ... @*/`.

JML uses syntax quite similar to Java.

    //@ensures x > 0;
    //@requires Long.MIN_VALUE <= sum && sum <= Long.MAX_VALUE;
    /*@maintaining sum == 
        (\sum int j; 
            i <= j && 0 <= j && j < a.length;
            (\bigint)a[j]
        );
    @*/
    
You can you Java's boolean logic `a && b`, `a || b` and so on. You must separate lines with `;`. You can even use `public`, `private` and `protected`.However, additionally you get a bunch of new keywords like `ensures`, `requires`, `maintaining` and many more. 

###Most essential tools
#####requires + ensures

The core concept of JML is to make sure that methods work correctly. This is what `ensures` and `requires` are all about. Check this out:

    //@requires a>7
    //@requires b>4
    //@ensures \result>13
    int plus(int a, int b){
        return a+b+1;
    }
    
It essentially says:

Method `plus(int a,int b)` requires you to pass arguments such that `a` is greater than 7, and `b` is greater than 12. If you do that, then you can be 100% sure that the result (represented by `\result`) is greater than 13. 

Now, give yourself some time and think: Is this code correct? If you say "yes", then you are terribly wrong. Copy this code and try to run `openjml -esc YourFile.java`. You will see an error mentioning integer overflow. OpenJML is really smart. It take everything into account. Even such extreme scenarions where sum of two really huge integers causes overflow in modular arithmetic. 

#####invariant

#####assert




