# lioliosh
A Lisp implementation in C

## Built-in functions
Builtin functions are predefined and operate on a list of arguments. If you have a builtin function named `foo` that you could evaluate with the arguments `bar`, `'(bar baz)` and `'(bar (baz bar))`, then the following code will perform the operation and return its output.
```
>>> foo bar '(bar baz) '(bar (baz bar))
```
#### Arithmetic operators
###### Cons
Cons prepends an atom to a quoted expression to return a quoted expression.
```
>>> cons 1 '(2 3 4)
'(1 2 3 4)
```
###### Len
Len takes a quoted expression and returns its length
```
len '(1 2 3 4)
4
```
###### Init
Init takes a quoted expression and returns everything but the last element
```
init '(1 2 3 4)
'(1 2 3)
```