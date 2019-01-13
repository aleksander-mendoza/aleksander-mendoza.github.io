#Side effects

As you know, Java is not a functional language and we have to deal with side effects while proving Java code. It's very easy to dismiss all the possibilities that arise from it. Here is an example:

    Camera camera;
    
    void stopCamera(){
        if(camera!=null){
            camera.stopPreview();
            camera.release();
            camera = null;    
        }
    }
    
What do you think is wrong with this code? If you try to verify it, checker will tell you something like 

> The prover cannot establish an assertion (PossiblyNullDeReference) in method

That is because you checked 

    if(camera!=null){
        
but can you be 100% sure that 

    camera.stopPreview()
    
doesn't look like this?

    void stopPreview(){
        camera = null;
    }
    
The problem is that our example `camera` is an instance variable which makes it theoretically global and vulnerable to side effects. These kinds of errors seem really unintuitive. There are many error such strange messages. At first make you think that JML is flawed but after deeper investigation you actually see taht JML was righ all along. 99.9% of time it's you - the programmer - who misses something. 

Alright, but how to solve problem of side effects? Here are a few approaches:

###ensures + \old

Specify

    //@ensures camera == \old(camera);
    void stopPreview(){
        ...
    }

###assignable

No external variables can be assigned:

    //@assignable \nothing;
    void stopPreview();

Only specific external variables can be assigned:

    //@assignable var0;
    //@assignable var1;
    void stopPreview();

###pure

Nothing from outside can be used at all. No reading. No writing. Only input are parameters and only output is the returned value.

    /*@ pure @*/ void stopPreview();

###invariant

You can go even farther and just mark some invariant contraints for specific variable. They will then hold across all methods and classes and if any method attemps to violate them, an error will be returned.

    //@public invariant camera != null;
    
Notice that this specific `!= null` inavriant can be simplified with a shorthand:

    public /*@ non_null @*/ Camera camera;
    
(and in most cases it's not even necessary because `non_null` is always default in OpenJML and to make something nullable you must explicity specify it with `/*@ nullable @*/`)
