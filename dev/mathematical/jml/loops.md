#Loops

JML has some special keywords to manage loops. Prooving loop correctness can sometimes require a bit more attention. Take a look at this example:

    public void f() {
        
        for (int i = 1; i <= 100; i++) {
            //@assert i > 0;
        }
    }
    
It's quite obvious that i>0. JML won't know it unless you explicitly point it out. You must add `maintaining` to make it work:
    
    public void f() {
        //@maintaining i>0;
        for (int i = 1; i <= 100; i++) {
            //@assert i > 0;
        }
    }
    
This special keyword works similarly like `ensures` and `requires` except that it's used to specify behaviour of loops instead of functions.
