---
layout: post
title:  "Hoare Logic (JML Intro)"
date:   2020-04-18 14:32:58 +0200
categories: hoare
permalink: /jml_intro.html
---

Perhaps the most important part of formal verification is creating specification with Hoare logic. We will show how it works with help of Java Modeling Language implemented by [OpenJML](http://www.openjml.org). First go to [OpenJML's download page](http://www.openjml.org/downloads/), get their bundle, unzip it and save somewhere. We will need `openjml.jar` from that bundle later. Next create a java file named `Hoare.java`. I will show you what JML is all about with a simple example. 

Imagine you are working for a NASA and your task is to detect layers of planets. Assume that there are 3 layers: mantle, outer core and inner core. Each of them has some specific size. NASA will use their radars to scan deeply into the ground and your software has to tell them which layer they reached. Your code might look like this:
{% highlight Java %}

public class Hoare {

    public static final int MANTLE = 0;
    public static final int OUTER_CORE = 1;
    public static final int INNER_CORE = 2;

    int findLayer(int mantleSize, int outerCoreSize, 
            int innerCoreSize, int scannedDepth) {
        if (scannedDepth < mantleSize) {
            return MANTLE;
        }
        if (mantleSize < scannedDepth && 
                scannedDepth < mantleSize + outerCoreSize) {
            return OUTER_CORE;
        }
        if (mantleSize + outerCoreSize < scannedDepth && 
                scannedDepth < mantleSize + outerCoreSize + innerCoreSize) {
            return INNER_CORE;
        }
        return -1; //impossible
    }
}

{% endhighlight %}

This method takes sizes for consecuive layers as parameters `mantleSize`, `outerCoreSize`, `innerCoreSize`, and then checks which one was reached by radar's `scannedDepth`. However, did you notice that last return statement? If the system is correct, then it should *never* output `-1`, because we assume that radar **cannot** scan deepr than the centre of planet. In JML we can express it with 

{% highlight Java %}
//@requires scannedDepth < mantleSize + outerCoreSize + innerCoreSize;
int findLayer(int mantleSize, int outerCoreSize, 
            int innerCoreSize, int scannedDepth) { ...}
{% endhighlight %}

JML will read comments that start with `//@` and validate them again your code. (Pay close attention to **whitespaces**! If you start your comment with `// @` JML will **not** read it.) Try running openjml against `Hoare.java`

{% highlight Bash %}
java -jar /path/to/openjml.jar -code-math bigint -esc Hoare.java
{% endhighlight %}

If you did everything correctly, this command should take a couple of seconds and then terminate without printing anything at all. However, if you made some mistake, you will see a list of errors. Notice the argument `-code-math bigint`. Is allows you not to worry about modular arithmetic and integer overflows. If you removed this option and ran only:

{% highlight Bash %}
java -jar /path/to/openjml.jar -esc Hoare.java
{% endhighlight %}

you would see several errors:

{% highlight Java %}

openjml -esc Hoare.java 
Hoare.java:20: warning: The prover cannot establish an assertion (ArithmeticOperationRange) in method findLayer:  overflow in int sum
                scannedDepth < mantleSize + outerCoreSize + innerCoreSize) {
                                                          ^
Hoare.java:19: warning: The prover cannot establish an assertion (ArithmeticOperationRange) in method findLayer:  underflow in int sum
        if (mantleSize + outerCoreSize < scannedDepth && 
                       ^
Hoare.java:16: warning: The prover cannot establish an assertion (ArithmeticOperationRange) in method findLayer:  underflow in int sum
                scannedDepth < mantleSize + outerCoreSize) {
                                          ^
Hoare.java:19: warning: The prover cannot establish an assertion (ArithmeticOperationRange) in method findLayer:  overflow in int sum
        if (mantleSize + outerCoreSize < scannedDepth && 
                       ^
Hoare.java:16: warning: The prover cannot establish an assertion (ArithmeticOperationRange) in method findLayer:  overflow in int sum
                scannedDepth < mantleSize + outerCoreSize) {
                                          ^
5 warnings

{% endhighlight %}

As you can see, OpenJML actually noticed possible source of bugs that normally doesn't even pass through programmer's mind. In fact, you shouldn't ignore those errors! Remember the story of Ariane 5 spacecraft explosion. If you work for NASA, you better not make such mistakes. However, here for the purpose of tutorial and simplicity let's just use `-code-math bigint`.

So after, having this figured out, one might think that the method will never return `-1`. Let's ask JML and see what is thinks about it. We can indeed express such assertion with

{% highlight Java %}
//@requires scannedDepth < mantleSize + outerCoreSize + innerCoreSize;
//@ensures \result != -1;
int findLayer(int mantleSize, int outerCoreSize, 
            int innerCoreSize, int scannedDepth) { ...}
{% endhighlight %}

The `ensures` annotation, instructs JML to check if result of method denoted by `\result` satisfies certain constraints. JML also assumes that programmer never calls the method with invalid parameters and  anything provided with `requires` annotation always holds true.

Unfortuantelly after running `openjml.jar` again, we see the following errors:

{% highlight Java %}
Hoare.java:24: warning: The prover cannot establish an assertion (Postcondition: Hoare.java:10: ) in method findLayer
        return -1;
        ^
Hoare.java:10: warning: Associated declaration: Hoare.java:24: 
    //@ensures \result != -1;
       ^
2 warnings 
{% endhighlight %}

Let's see if there is something that we forgot about. For instance what if any of the sizes is negative? Even though we know that it should never happen, JML doesn't trust us too much. And we shouldn't trust our gut either. Let's make our assumption explicit with:


{% highlight Java %}
//@requires mantleSize > 0;
//@requires outerCoreSize > 0;
//@requires innerCoreSize > 0;
//@requires scannedDepth > 0;
//@requires scannedDepth < mantleSize + outerCoreSize + innerCoreSize;
//@ensures \result != -1;
int findLayer(int mantleSize, int outerCoreSize, 
       int innerCoreSize, int scannedDepth) {...}
{% endhighlight %}

Rerun OpenJml and see what it says now. Unfortunately JML will reject our code and print the exact same error messages as before. So what is the matter? Whenever you run out of ideas of what might be wrong it's worth adding extra assertions like this:

{% highlight Java %}

//@requires mantleSize > 0;                                                                                        
//@requires outerCoreSize > 0;                                                                                     
//@requires innerCoreSize > 0;                                                                                     
//@requires scannedDepth > 0;                                                                                      
//@requires scannedDepth < mantleSize + outerCoreSize + innerCoreSize;                                             
//@ensures \result != -1;                                                                                          
int findLayer(int mantleSize, int outerCoreSize, 
        int innerCoreSize, int scannedDepth) {  
    //@assert scannedDepth >= 0;                                                                                                         
    if (scannedDepth < mantleSize) {                                                                                
        return MANTLE;                                                                                              
    }                                                                                                               
    //@assert scannedDepth >= mantleSize;                                                                          
    if (mantleSize < scannedDepth && 
            scannedDepth < mantleSize + outerCoreSize) {                                   
        return OUTER_CORE;                                                                                          
    }                                                                                                               
    //@assert scannedDepth >= mantleSize + outerCoreSize;                                                         
    if (mantleSize + outerCoreSize < scannedDepth && 
            scannedDepth < mantleSize + outerCoreSize + innerCoreSize) {   
        return INNER_CORE;                                                                                          
    }                                                                                                               
    //@assert scannedDepth >= mantleSize + outerCoreSize + innerCoreSize;                                          
    return -1;                                                                                                      
}                                                                                                                   
{% endhighlight %}

You can add `//@assert` annotations anywhere you wish. When you use them together with `ensures` and `requires` their persence doesn't change much. It only adds extra checks, which can generate more error logs (this is exactly what we are hoping for). However, there is one more application for `assert` annotations - to generate unit tests. For instance imagine you are given a large code-base that has been written without any formal methods. You might still annotate some parts of the code with such assertions and then OpenJML will generate very deep unit tests for you. It will even try to find all possible corner cases and most pessimistic inputs that could be. Anyway, let's go back to our code and see OpenJML's error logs this time:

{% highlight Java %}

Hoare.java:26: warning: The prover cannot establish an assertion (Assert) in method findLayer
      //@assert scannedDepth >= mantleSize + outerCoreSize ;                                                         
         ^
Hoare.java:31: warning: The prover cannot establish an assertion (Assert) in method findLayer
      //@assert scannedDepth >= mantleSize + outerCoreSize + innerCoreSize;                                          
         ^
2 warnings
{% endhighlight %}

As you can see, those logs are much more informative than previous ones. The first two asserions passed successfully, and only the third and fourth assertion failed. This means that the line causing trouble should be 
{% highlight Java %}
if (mantleSize < scannedDepth && 
              scannedDepth < mantleSize + outerCoreSize)
{% endhighlight %}

Indeed if you take a closer look at it, the assertion `scannedDepth >= mantleSize + outerCoreSize` doesn't hold because if the corner case when `scannedDepth == mantleSize `. So this tells us we should change our code to something like this:

{% highlight Java %}

//@requires mantleSize > 0;                                                   
//@requires outerCoreSize > 0;                                                
//@requires innerCoreSize > 0;                                                
//@requires scannedDepth > 0;                                                 
//@requires scannedDepth < mantleSize + outerCoreSize + innerCoreSize;        
//@ensures \result != -1;                                                     
int findLayer(int mantleSize, int outerCoreSize,                              
        int innerCoreSize, int scannedDepth) {                                
    if (scannedDepth < mantleSize) {                                          
        return MANTLE;                                                        
    }                                                                         
    //@assert scannedDepth >= mantleSize;                                     
    if (mantleSize <= scannedDepth &&                                         
            scannedDepth < mantleSize + outerCoreSize) {                      
        return OUTER_CORE;                                                    
    }                                                                         
    //@assert scannedDepth >= mantleSize + outerCoreSize ;                    
    if (mantleSize + outerCoreSize <= scannedDepth &&                         
            scannedDepth < mantleSize + outerCoreSize + innerCoreSize) {      
        return INNER_CORE;                                                    
    }                                                                         
    //@assert scannedDepth >= mantleSize + outerCoreSize + innerCoreSize;   
    return -1;                                                                
}                                                                             
{% endhighlight %}


This time, we will see a very interesting error:

{% highlight Java %}
Hoare.java:30: warning: There is no feasible path to program point before explicit assert statement in method T.Hoare.findLayer(int,int,int,int)
      //@assert scannedDepth >= mantleSize + outerCoreSize + innerCoreSize;   
         ^
1 warning
{% endhighlight %}

OpenJML actually found out that there is no possible way, our program could step over the third if-statement and reach the assertion
{% highlight Java %}
//@assert scannedDepth >= mantleSize + outerCoreSize + innerCoreSize;
{% endhighlight %}
If you think about it, that's actually a good sign and not any real error at all. All that's left to do is to remove the last assertion. Our end result should be this:

{% highlight Java %}
public class Hoare {

    public static final int MANTLE = 0;
    public static final int OUTER_CORE = 1;
    public static final int INNER_CORE = 2;

    //@requires mantleSize > 0;                                                   
    //@requires outerCoreSize > 0;                                                
    //@requires innerCoreSize > 0;                                                
    //@requires scannedDepth > 0;                                                 
    //@requires scannedDepth < mantleSize + outerCoreSize + innerCoreSize;        
    //@ensures \result != -1;                                                     
    int findLayer(int mantleSize, int outerCoreSize,                              
            int innerCoreSize, int scannedDepth) {                                
        if (scannedDepth < mantleSize) {                                          
            return MANTLE;                                                        
        }                                                                         
        //@assert scannedDepth >= mantleSize;                                     
        if (mantleSize <= scannedDepth &&                                         
                scannedDepth < mantleSize + outerCoreSize) {                      
            return OUTER_CORE;                                                    
        }                                                                         
        //@assert scannedDepth >= mantleSize + outerCoreSize ;                    
        if (mantleSize + outerCoreSize <= scannedDepth &&                         
                scannedDepth < mantleSize + outerCoreSize + innerCoreSize) {      
            return INNER_CORE;                                                    
        }                                                                          
        return -1;                                                                
    }                                                                                                             
}
{% endhighlight %}

Now OpenJML doesn't return any errors anymore. We are done making our first mathematically proven code! 

In the upcoming posts I will show you more advanced JML and explain the mathematics behind Hoare logic.