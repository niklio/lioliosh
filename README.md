# lioliosh
A Lisp implementation in C

## Getting Started
##### Paste the code into terminal:
Xcode (an OS X specific coding tool)
```
$  xcode-select —install
```
Homebrew (a package manager that lets you install stuff you need for coding that is not already on you computer like programming languages)
```
$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)”
```
Git
```
$  brew install git
```
Downloads code from github onto your computer
```
$ git clone https://github.com/nliolios24/lioliosh
```
Compiles the c code into an executable
```
$ cc -std=c99 -Wall repl.c mpc.c -ledit -lm -o lioliosh
```
Runs the repl
```
./lioliosh
```

## Built-in functions
Builtin functions are predefined and operate on a list of arguments. If you have a builtin function named `foo` that you could evaluate with the arguments `bar`, `'(bar baz)` and `'(bar (baz bar))`, then the following code will perform the operation and return its output.
```
>>> foo bar '(bar baz) '(bar (baz bar))
```
To define a variable
```
>>> def '(x y) 1 2
```
To create a function that takes input x
```
>>> fn '(cube z) '(^ x 3)
```
To call a function
```
>>> cube y
8
```
#### Math Functions
The supported math functions are:
```
+ - * / % ^ 
```
They each take one or more arguments.
#### Comparison Functions
The Supported comparison functions are:
```
== != > < >= <=
```
They each take two arguments.
#### If
If takes three arguments, a condition and two quoted expressions. If the condition is nonzero, then the first quoted expression is evaluated. Otherwise, the second is evaluated.
```
>>> if (5) '(10) '(2)
2
```
#### List operators
###### List
List takes n arguments are returns a quoted expression.
```
>>> list 1 2 3
'(1 2 3)
```
###### Eval
Eval takes a quoted expression and returns n arguments.
```
>>> eval '(1 2 3)
1 2 3
```
###### Head
Head returns the first element of a quoted expression
```
>>> head '(1 2 3)
'(1)
```
###### Tail
Tail returns a quoted expression all but the first element of a quoted expression.
```
>>> tail '(1 2 3)
'(2 3)
```
###### Join
Join joins two quoted expressions.
```
>>> join '(1 2) '(3 4)
'(1 2 3 4)
```
###### Min
Min returns the smallest element in a quoted expression.
```
>>> min '(1 2 3)
1
```
###### Max
Max returns the largest element in a quoted expression.
```
>>> max '(1 2 3)
3
```
###### Cons
Cons prepends an atom to a quoted expression to return a quoted expression.
```
>>> cons 1 '(2 3 4)
'(1 2 3 4)
```
###### Len
Len takes a quoted expression and returns its length.
```
>>> len '(1 2 3 4)
4
```
###### Init
Init takes a quoted expression and returns everything but the last element.
```
>>> init '(1 2 3 4)
'(1 2 3)
```
###### Last
Last takes a quoted expression and returns the last element.
```
>>> last '(1 2 3 4)
4
```
###### Nth
Nth takes a quoted expression and a number and returns the nth element. 
```
>>> nth 2 '(1 2 3 4)
3
```
###### Drop
Drop takes a number and a quoted expression and returns a quoted expression with the first n elements removed.
```
>>> drop 2 '(1 2 3 4)
'(4)
```
###### Take
Take takes a number and a quoted expression and returns the first n elements.
```
>>> take 2 '(1 2 3 4)
'(1 2)
```
###### Elem
Elem takes a number and quoted expression and chacks if the number is in the quoted expression. If it is it outputs one, and if it isn't it outputs zero.
```
>>> elem 5 '(1 2 3 4)
0
```
#### Logical Operations
###### Not
Not takes a number and returns false if the number is zero and true if the number is not zero.
```
>>> not 5
true
```
###### All
All takes a quoted expression and returns true if all numbers in the quoted expression are not zero and false if any are zero.
```
>>> all 1 2 0 3
false
```
###### Any
Any takes a quoted expression and returns true if any numbers in the quoted expression are not zero and false if all of the numbers are zero.
```
>>> any 0 0 1 2
true
```

## Motivation
Hanan and I built Lioliosh in an effort to elucidate the connection that Gary William Flake draws between the Turing Machine and lambda calculus in his book, The Computational Beauty of Nature. Summarizing the Church-Turing thesis, Flake states "Any function that can be called *computable* can be computed by lambda calculus, a Turing machine, or a general recursive function."(p30) Lioliosh meets the Church-Turing standard for computibility by implementing features proposed by Alonzo Church's lambda calculus (like function composition) and providing built-in functions to implement all of Flake's general recursive rules.
While defining a general recursive function, Flake enumerates necessary rules which he calls *Zero*, *Successor*, *Projection*, *Composition*, and *Recursion*. Lioliosh supports the implementation of all five with its built-ins.
Here is a quick Lioliosh implementation of Flake's general recursive rules.
```
; Let Z be the zero function
(fn '(Z x) '(0))


; Let S be the successor function
(fn '(S x) '(x + 1))


; Let P_i be the projection function which returns its ith argument (1 indexed).
; First we will define i arbitrarily.
(def '(i) 5) 
; Now we can define a general projection function P which takes the desired index as its first argument.
(fn '(P i x : xs) 
    '(if (== i 1)
        '(x)
        '(curry (P (- i 1)) xs)))
; We could match Flake's syntax by defining i to an arbitrary value outside of P and creating a P_i function like this:
(def '(i) 3)
(fn '(P_i : xs) '(curry (P i) xs))


; Function composition is built into the language in a more direct way.
; Lets define two simple arithmetic functions, g and h.
(fn '(g x) '(- (* x 2) 5))
(fn '(h x) '(/ (+ x 10) 2))
; Compose f(g(5)) by running either of the following
(f (g 5))
(comp h g 5)


; We will define a function f according to Flake's definition of recursion.
; Using the functions g and h which we defined above, we will implement a function f such that:
; f(x, 0) = g(x), and f(x, y+1) = f(h(x), y).
(fn '(f x y)
    '(if (== y 0)
        '(g x)
        '(f (h x) (- y 1))))
```
The function `f` is pretty meaningless the way we've defined it, but these 5 rules demonstrate that Lioliosh can compute anything which William Flake calls a general recursive function. This also demonstrates that Lioliosh is computationally equivalent to a Turing machine, lambda calculus, and general recursion.

## Execution
The Lioliosh compiler executes code by parsing a statement according to parenthesis, constructing an Abstract Syntax Tree (AST), then evaluating the tree from the bottom up (from leaves to trunk). Every statement in Lioliosh is comprised of nested expressions. For an expression to be evaluated, its first element must be a function which is able to evaluate the following elements in the expression.
We will use multiplication as our example. The operator `*` takes a variable number of arguments and multiplies them together.

```
>>> (* 1 2 3 4 5)
120
```

Lioliosh does not actually implement its own arithmetic operators (although Flake provides a demonstration of a recursive multiplication routine on page 39) so in this case, Lioliosh used the binary C implementation of multiplication to evaluate the list of arguments like this: `(((1 * 2) * 3) * 4) * 5`

Now that we have a sense of how Lioliosh parses a simple expression, lets try something more complicated.
We're going to computes the average of several numbers.

```
>>> (/ (+ 1 2 3 4) 4)
2.5
```

When executing this nested expressions, The AST is constructed as follows:
```
         '/'
        /   \
      '+'    4
    / | | \
    1 2 3 4
```

Lioliosh evaluates the expression `(+ 1 2 3 4)` first. This simplifies the AST:
```
    '/'
   /   \
  10    4
```
Division is evaluated just like multiplication, returning the correct result of `2.5`.

The Lioliosh compiler is responsible for converting a statement into an evaluable AST. The compiler implements a few basic types – decimal values, lists and strings, and a minimal set of operations such as addition, multiplication, head and tail.
The rest of Lioliosh is implemented in Lioliosh and loaded at runtime from the `src/prelude.lio` file. The compiler is responsible for calling functions from prelude, but many redundant functions (like `last`, which returns the last element of a list), are implemented in Lioliosh to reduce the size of the compiler.

Having built a sufficient standard library for the existing compiler, new functionality, such as garbage collection or a type system, needs to be added to the compiler to expand the language.

## Findings
Though Lioliosh can compute anything that is theoretically computable, we learned that some things are really hard to do because of how the language is set up. We ran into problems when we tried to implement Flake's L-System.

##### Error logging:
Since the language has no error logging, when code doesn’t work it's difficult to find errors in the code. For example, when there is a syntax error, Lioliosh returns:
```
error: expected whitespace, '-', one or more of one of '0123456789', one or more of one of 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_+^%%-*/\=<>!&:', '"', ';', '(', ''' or ')' at end of input
```
A more fully featured language returns a more specific error messages. For example, when there's a syntax error in python code, python returns:
```
>>> )
  File "<stdin>", line 1
    )
    ^
SyntaxError: invalid syntax
```
This is a much more helpful error message because it points to exactly what and where the error is.


##### Data type system:
Lioliosh also lacks a type system. A type system assigns a property called a type to each piece of data to reduce the chance of bugs. This could prevent you from passing a number into the length function instead of a list. An implementation of L Systems would have been about 50 lines of code. In there, it's easy to get confused about the type of data a function accepts or returns and pass the wrong type of data into a function, causing a fatal error. 

Lioliosh is also a comparatively slow language. Two optimizations which could speed up runtime are tail recursion and garbage collection.


##### Tail recursion:
In recursion you often have to call the function you are defining on the tail of a list. Every time a function is created, you have to allocate some memory to a function and every time a function is called, it costs some memory. In Lioliosh when you call `(+ 10 9)`, the program creates nine functions, creating a new function each time you call the function on the tail causing the compiler to allocate unnecessary memory. Tail call recurion squashes these nine functions into one funtion to save memory.


##### Garbage Collection:
Garbage collection is memory recycling. When you define a variable, it takes up memory. Garbage collection saves memory by deleting past values of variables once they are redefined. Lioliosh does not have this, so if you defined a variable a to the number 10 and then redefined it to equal 11, the language would continue storing the value 10 even though it is no longer being used. This means that an implementation of L Systems in lioliosh would have had a very long run time.
