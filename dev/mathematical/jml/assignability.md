#Assignability

OpenJML has special keyword `assignable` that restricts what variables you are allowed to mutate. It's a bit tricky to understand sometimes. Consider this:

    public class E{
        public int f;
    }
    
    
    //@assignable e.f;
    public void f(E e, E e2) {
        e.f = 4; //this is okay
        e = new E(); //this is okay too
        e2.f = 3; //this is not okay
        e2 = new E(); //this is okay
    }
    
You might think that by specifying only `e.f` as assignable, doing something like `e = new E()` would fail. But actually that's not what `assignable` does. In reality it only restricts you from mutating global and instance varaibles. Local varaibles and parameters are okay to mutate. However, field `f` is not a parameter! It's a completely external varaible and that's why `e2.f = 3` fails! Here is an extensive example:



    int g;
    
    public class E{
        public int f;
    }
    
    public E n = new E();
    
    //@assignable e.f;
    public void f(E e, E e2,int h) {
        e.f = 4; //ok
        e = new E(); //ok
        e2.f = 3; //not ok
        e2 = new E(); //ok
        h = 4; //ok
        g = 3; //not ok
        SomeGlobal.staticInt = 5; //not ok
        n.f = 10; //not ok
        n = new E(); //not ok
    }
    