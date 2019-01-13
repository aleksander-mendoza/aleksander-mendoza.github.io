# Java good practices

Here I will try to show you some pros and cons of many many Java features. Whenever I write something I think about 3 very important things:

- is it easy to change and customize?
- is it difficult to misuse? (in other words: is the interface intuitive?)
- is it dead simple?

Your code is well written only if you can shout **hell yes!** to all the questions above.

### Singletones vs _static_

Let's say you've got a certain resource like Keyboard. Normally people have only one so it's pretty safe to assume you need just single instance:

	public class Keyboard {
		public static int queryLastPressedKey() {
			...	
		}
	}
	
What do you think about this class? It seems easy to use and simple. But wait! One day you get visited by some freak who uses 2 keyboards. What now? Oh, I've got this. Let's do this (I'm being serious, that this is genuinely an approach I've seen some people take):

_Keyboard.java:_

	public class Keyboard {
		public static int queryLastPressedKey() {
			...	
		}
	}
	
_AdditionalKeyboard.java:_

	public class AdditionalKeyboard {
		public static int queryLastPressedKey() {
			...	
		}
	}
	
This design is plain terrible. You should never do this. Why? Because everytime you want to change something in Keyboard you need to remember about changing it in AdditionalKeyboard too.

> You should never have to "just remember to change X whenever you change Y". I could bet million dollars that one day you will forget about X. And the most evil thing that could ever happen to you is when you forget but your code compiles and even runs most of the time just fine. Except one day you will get a crash and debugging it will take you a week.

A much better approach is to write this:

	public class Keyboard {
		public static final Keyboard singleton = ...;
		
		private Keyboard(...){
			...
		}
		
		public int queryLastPressedKey() {
			...	
		}
	}

And whenever you get asked to actually add second instance for any reason, you can just make a tiny change:

	public class Keyboard {
		
		/**Official/Main instance*/
		public static final Keyboard singleton = ...;
		
		/**Make yourself extra instances if you really need to*/
		public static Keyboard factory(...){
			...
		}
		
		private Keyboard(...){
			...
		}
		
		public int queryLastPressedKey() {
			...	
		}
	}

### _Exception_ vs error code

People who love C often think that error codes are cool. Except they aren't.

	/**Returns error code:
	0 - means success
	1 - means foo happened
	2 - means bar happened
	*/
	public int runFastAlgorithm(String s) {
		...
	}

Then I try to use this algorithm:

	switch(runFastAlgorithm(...)){
		case 0:
			...
		case 1:
			...
		case 2:
			...
		default:
			...
	}

Seems good, right? Okay, but after 2 years you look at that algorithm again and see a mistake. You modify it:

	/**Returns error code:
	0 - means success
	1 - means foo happened
	2 - means bar happened
	3 - means fiz happened
	*/
	public int runFastAlgorithm(String s) {
		...
	}
	
and you forgot about the fact that I (and 10 other people too) used your algorithm somewhere else in the code. Now you better hope that everyone had forseen such scenario and their code is tolerant for new error codes. 

In Java you should always use excpetions! They have the following advantages:

- Many exceptions can be checked at compile time! It means that if you ever forget to catch them, java will tell you about it. 
- Even if in case of _RuntimeException_s you are still allowed to specify them as part of function signature:

		public void runFastAlgorithm(String s) throws NoSuchElementException{
			...
		}
		
	and then, everyone who sees your function, can immediately guess that a ``try`` ``catch`` is needed.
	
- Every caller of your function can decide to throw too. They don't need to actually deal with your exception. While it's also possible with error codes, it's not a great experience.

### Pooling vs _InterruptedException_

Consider this code:

	public class MyThread extends Thread
	{
		public boolean isRunning = true;
		@Override
		public void run() {
			while(isRunning) {
				doStuff();
			}
			//exit cleanly	
		}   
	}

This a very common idiom among Java programmers... except it's a bad one. Setting ``isRunning`` is a safe way of exiting the loop. Instead use this:

	public class MyThread extends Thread
	{
		public boolean isRunning = true;
		@Override
		public void run() {
			while(isRunning) {
				doStuff();
				try {
					Thread.sleep(100);
				} catch (InterruptedException e) {
					isRunning = false; // or break;
				}
			}
			//exit cleanly	
		}   
	}
	
	
Then from another thread you can just do:

	MyThread t = new MyThread(...);
	t.run();
	...
	t.interrupt();
	
This is exactly what ``InterruptedException`` is for. Alternatively instead of ``Thread.sleep(100);`` you might also use 

	while(isRunning && !Thread.interrupted()){...}



	