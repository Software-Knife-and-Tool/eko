---
title: print library
---

@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Compiler

@s Summary
<hr><br/>
<p></p>
The *core* compiler accepts all legal *libmu* forms by the simple expedient of deferring
any form it doesn't recognize to the runtime compiler. In addition, it adds defining macros
and a lambda list with a rest argument.
<br/>

@s Compiling an environment
<hr>
<p></p>
***core::compile-add-env*** *function* *env* ***=>*** *list*
<br/><br/>
`core::complle-add-env` converts a *function* to a *frame-descriptor* and adds it to
a list of *frame-descriptors*.

<br/>
<p></p>

[*frame-descriptor*]
<br/>
>
> `(frame-id . ((symbol . offset)...))`
>
<p></p>

`arguments:`
>
> `fn:` function
>
> `env:` lexical environment

`returns:` amended list of frame-descriptors

--- compile-env
(:const core::frame-descriptor
   (:lambda (fn symbols)
      (mu:cons fn symbols)))

(:const core::frame-fn (:lambda (desc) (mu:car desc)))
(:const core::frame-id (:lambda (desc) (core::fn-frame-id (core::frame-fn desc))))
(:const core::frame-symbols (:lambda (desc) (mu:cdr desc)))

(:const core::compile-add-env
  (:lambda (fn env)
     (core:assert core:functionp fn "core::compile-add-env requires a frunction")
     (core:assert core:listp env "core::complie-add-env requires a list")  
     (mu:cons
       (core::frame-descriptor
         fn
         (core:foldl
           (:lambda (sym acc) (mu:cons (mu:cons sym (mu:length acc)) acc))
           ()
           (mu:car (core::fn-form fn))))
       env)))
---

@s Special Forms
<hr>
<p></p>
***core::lambda*** *function* *env* ***=>*** *list*
<br/><br/>
`core::macroexpand` expands a macro definition against a list of arguments.

CLHS adds an environment argument here, think about why.
<br/>
<p></p>

`arguments:`
>
> `form:` whole form

`returns:` qualified symbol or *frame-ref*

--- compile-specials
(:const core::quote (:lambda (form env) form))

(:const core::lambda
   (:lambda (form env)
      (core::compile-lambda
        (mu:cons (mu:car (mu:cdr form)) (mu:cons (mu:cdr (mu:cdr form)) ()))
        env)))

(:const core::defconst
   (:lambda (form env)
      ((:lambda (symbol value)
          (core:check-type symbol :symbol "core::defconst requires a symbol")
          (mu::compile
            (mu:cons
              :const
              (mu:cons
                (mu::compile symbol)
                (core:mapcar (:lambda (el) (core:compile el env)) value)))))
       (mu:nth 1 form)
       (mu:nthcdr 2 form))))

(:const core::defmacro
   (:lambda (form env)
      ((:lambda (symbol lambda)
          (core:check-type symbol :symbol "core::defmacro requires a symbol")
          (core:assert core:listp lambda "core::defmacro requires a lambda list")
          (mu::compile
            (mu:cons
              :const
              (mu:cons
                (mu::compile symbol)
                (mu:cons (core::compile-macro lambda env) ())))))
       (mu:nth 1 form)
       (mu:nthcdr 2 form))))

(:const core::defun
   (:lambda (form env)
      ((:lambda (symbol lambda)
          (core:check-type symbol :symbol "defun requires a symbol")
          (core:assert core:listp lambda "defun requires a lambda list")
          (core:compile
            (mu:cons :const
              (mu:cons symbol
                (mu:cons (core::lambda (mu:cons :lambda lambda) env) ())))
            env))
       (mu:nth 1 form)
       (mu:nthcdr 2 form))))

;;; this if can take two or three forms
(:const core::if
   (:lambda (form env)
      (core:assert core:listp env "core::if requires a list")
      (core:assert (:lambda (form) (mu:fixnum< 2 (mu:length form))) form "core:if takes two or three arguments")
      (core:assert (:lambda (form) (mu:fixnum< (mu:length form) 5)) form "core:if takes two or three arguments")
      (core:compile
        ((:lambda (thunk if-impl)
            (mu:cons 'core:funcall
              (mu:cons (thunk
                         (mu:cons if-impl
                           (mu:cons (core:compile (mu:nth 1 form) env)
                             (mu:cons (thunk (mu:nth 2 form))
                               (mu:cons (thunk (mu:nth 3 form))
                                 ())))))
              '(()))))
         (:lambda (form)
            (core:compile
              (mu:cons 'lambda
                (mu:cons ()
                  (mu:cons form
                    ())))
              env))
         (:lambda (term true-fn false-fn)
            (:if term
                 (core:funcall true-fn ())
                 (core:funcall false-fn ()))))
        env)))

(:const core::env
   (:lambda (form env)
      (mu:cons :quote (mu:cons env ()))))

(:const core::special
   (:lambda (symbol)
      (core:assoc
        symbol
        '((defconst    . core::defconst)
          (defun       . core::defun)
          (defmacro    . core::defmacro)
          (if          . core::if)
          (lambda      . core::lambda)
          (capture-env . core::env)
          (:quote      . core::quote)))))
---

@s The Core compiler
<hr>
<p></p>
<br/>
`core:compile` recognizes a *core* lambda form and hands it to `core::compile-lambda`. The *core* lambda form adds an optional `rest`
argument to *libmu* lambda form.
<br/><br/>
<p></p>

[*core lambda form*]
<br/>
>
> `(lambda ([symbol ...] [&rest rest-symbol]) . body)`
>
<p></p>
Other list forms are handed to `core:compile` and atoms to `mu::compile`.
<br/><br/>

`arguments:`
>
> `form:` form to be compiled
>
> `env:` as-yet unused environment object

`returns:` *compiled form*

--- compile
(:const core::must-funcall
  (:lambda (fn)
     (:if (core::fn-lambda fn)
        (:if (core::fn-restp fn)
           :t
           (core:null (core::fn-unclosedp fn)))
        ())))

(:const core::make-funcall
   (:lambda (fn args env)
      (mu:cons 'core::fn-call
        (mu:cons fn
          (mu:cons
            (mu:cons :quote
              (mu:cons (core:mapcar (:lambda (el) (core:compile el env)) args)
                ()))
            ())))))

(:const core::unbound-qfn
   (:lambda (fn)
      (core:assert core:symbolp fn "core::unbound-fn requires a symbol")
      ;;; (core:assert (:lambda (fn) (core:null (mu:keyp fn))) sym "core::unbound-qfn illegal keyword")
      (:if (mu:boundp fn)
           ()
           (core:null (mu:eq (mu:sy-ns fn) (mu:find-ns ""))))))

(:const core::compile-activation
   (:lambda (fn-designator args env)
      ((:lambda (fn)
          (:if (core:null fn)
                 (:if (core::unbound-qfn fn-designator)
                      (core::make-funcall fn-designator args env)
                      (core:mapcar (:lambda (el) (core:compile el env)) (mu:cons fn-designator args)))
             (:if (core::fn-macrop fn)
                (core:compile (core:macroexpand (mu:cons fn args)) env)
                (:if (core::must-funcall fn)
                     (core::make-funcall fn args env)
                     (core:mapcar (:lambda (el) (core:compile el env)) (mu:cons fn args))))))
       (:if (core:functionp fn-designator)
          fn-designator
          (:if (core:symbolp fn-designator)
               (:if (mu:boundp fn-designator)
                    (mu:sy-val fn-designator)
                    ())
               (core:error fn-designator "core::compile-activation requires a function or symbol"))))))

(:const core:compile
   (:lambda (form env)
      (core:assert core:listp env "core:compile requires a list")
      (:if (core:consp form)
           ((:lambda (fn args)
               (:if (mu:keyp fn)
                  (mu::compile form)    ;;; libmu special form (:key ...)
                  (:if (core:consp fn)  ;;; implied application ((...) ...), native or core
                       (core:mapcar (:lambda (el) (core:compile el env)) form)
                       ((:lambda (specf)
                           (:if specf
                                (core:funcall (mu:sy-val specf) (mu:cons form (mu:cons env ())))
                                (core::compile-activation fn args env)))
                        (core::special fn)))))
            (mu:car form)
            (mu:cdr form))
         (:if (core:symbolp form)
              (core::compile-symbol form env)
              (mu::compile form)))))
---

--- core-compile --- noWeave
@{compile-specials}
@{compile-env}
@{compile}
---

--- core:compile.l --- noWeave
@{core-compile}
---