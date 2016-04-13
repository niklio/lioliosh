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
###### Pop
Pop takes a quoted expression and returns everything but the last element.
```
>>> pop '(1 2 3 4)
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
>>> all '(1 2 0 3)
false
```
###### Any
Any takes a quoted expression and returns true if any numbers in the quoted expression are not zero and false if all of the numbers are zero.
```
>>> any '(0 0 1 2)
true
```