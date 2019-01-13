# Ghosts

###What is it
Ghosts are variables that exist only in JML but are not seen by Java. You can declare them this way:

    //@ghost int integer;
    //@ghost int integerWithDefaultValue = 3;
    //@ghost SomeClass name;
    //@private ghost int privateInteger;
    //@public ghost int publicInteger;
    //@ghost int packageInteger;
    //@protected ghost int protectedInteger;

###What it's good for

#####Keeping track of meta information

Sometimes when proving your algorithms you might need to keep track of some meta information. A good example of it is ensuring state of your object:

    public class DataBase{
        //@public ghost boolean connected = false;
        
        //@requires connected == false;
        //@ensures connected == true;
        public void connect(){
            MySQL.connectStuff();
            //@set connected = true;
        } 
        
        //@requires connected == true;
        public String queryData(){
            return MySQL.queryData();
        }
    }
    
We could assume that `MySQL` is a library class that is not proved in any way but we have to use it. We will just assume that `MySQL` is implemented correctly and blindly trust it. But we can still create a wrapper `DataBase` that will take advanted of JML. 

We can also use ghost variables with interfaces.

    interface DataBase{
        //@public instance ghost boolean connected = false;
        //Notice that we can actually set default value!
       
        //@requires connected == false;
        //@ensures connected == true;
        public void connect();    
           
        //@requires connected == true;
        public String queryData();    
    }   
    
We have to use keyword `instance` when declaring ghost in interface because otherwise it would be static. Variables in interfaces are static by default!

#####Checking method call order

Another useful example is when you need to ensure that some method was called before other.

    //@ghost boolean calledA = false;

    //@ensures calledA == true;
    void A(){
        //@set calledA = true;
    }
    
    //@requires calledA == true;
    void B(){...}
    

    


