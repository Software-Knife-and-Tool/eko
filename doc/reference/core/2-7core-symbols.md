---
title: 6.2 Core Symbols
---

#### External Symbols

------

Core
````
   break   compile          eval           format     funcall
   load    macro-function   macroexpand    read       version
````

Sequences
````
   elt     sv-list
````

Lists

````
   assoc     dropl     dropr    findl-if    findr-if
   foldl     foldr     last     lastn       length
   mapc      mapcar    mapl     maplist     positionl
   positionr
````

Predicates
````
   charp     consp     doublep  exceptionp  fixnump
   floatp    functionp listp    namespacep  not
   null      sequencep streamp  stringp     symbolp
   vectorp   zerop
````

Exceptions
````
   error     error-unless  print-except  warn
````

Strings
````
   schar     string   string-append  string=  substr
````

#### Internal Symbols

------

Helpers
````
append
assert
break
````

Compiler
````
closure
compile-activation
compile-add-env
compile-closure
compile-lambda
compile-lambda-body
compile-macro
compile-symbol
complle-add-env
symbol-frame
core-lambda
core-macro
macroexpand-1
make-funcall
must-funcall
frame-descriptor
frame-fn
frame-id
frame-symbols
````

Special forms
`````
defconst
defmacro
defun
if
lambda
quote
special-table
````

Functions
````
clone
fn-arity
fn-call
fn-call-closure
fn-form
fn-frame-id
fn-lambda
fn-macrop
fn-name
fn-nreqs
fn-restp
fn-unclosedp
````

Lambda
````
lambda-closure
lambda-closuredesc
lambda-descriptor
lambda-env
lambda-macrop
lambda-nreqs
lambda-reqs
lambda-rest
lambda-syms
````

Quasiquote
````
quasi-comma
quasi-comma-at
quasi-list
quasiquote
````

Reader Implementation
````
read
read-atom
read-char-syntax
read-comment
read-consume-ws
read-dispatch
read-fixnum
read-fixnump
read-list
read-list-eol
read-macro
read-namespaces
read-quote
read-resolve-symbol
read-sharp
read-sharp-char
read-sharp-comment
read-sharp-symbol
read-sharp-vector
read-stream-designator
read-string
read-symbol-externp
read-symbol-keywordp
read-symbol-name
read-symbol-ns
read-whitespace
reader-stream
````
