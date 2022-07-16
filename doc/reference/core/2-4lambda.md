---
title: print library
---

@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Lambdas

@s Summary
<hr><br/>
<p></p>

The *core* compiler accepts all legal *libmu* forms by the simple expedient of deferring
any form it doesn't recognize to the runtime compiler. In addition, it adds defining macros
and a lambda list with a rest argument.
<br/>

@s Compiling a core lambda form
<hr><br/>
<p></p>
`core::compile-lambda validates the lambda list and adds it to the current lexical environment.
<br/><br/>

`arguments:`
>
> `form:` (lambda-list . body)
>

`returns:` *compiled form*
<br/><br/><hr>

<br/>
`core::compile-body` compiles a lambda body in that environment and returns a function that we
can clone with the compiled body. The cloned function has a *lambda-descriptor* that `mu:funcall`
does not recognize but *core:funcall* will.
<br/><br/>

`arguments:`
>
> `form:` a list of forms
>
> `env:` as-yet unused environment object

`returns:` compiled form
<br/><br/><hr>
<br/>

<p></p>
`core::core-lambda` compiles a *core* lambda list into a *lambda-descriptor*.
<p></p>

[*lambda descriptor*]
<br/>
>
> #(:t reqs restsym body macrop env closure)
>
> syms: list of symbols
> restsym: rest symbol
> macrop: () or :t
> env: closed symbols
> closure: closed symbol state
>
<p></p>

`arguments:`
>
> `form:` *lambda list*
>
> `env:` unused environment object

`returns:` *lambda-descriptor*
<br/><br/><hr>

--- lambda-desc --- noWeave
(:const core::lambda-descriptor
   (:lambda (reqs restsym macrop env closure)
      (mu:list-sv :t
        (mu:cons reqs
          (mu:cons restsym
            (mu:cons macrop
              (mu:cons env
                (mu:cons closure ()))))))))

(:const core::lambda-syms
  (:lambda (desc)
     (:if (core::lambda-rest desc)
       (core::append (core::lambda-reqs desc) (mu:cons (core::lambda-rest desc) ()))
       (core::lambda-reqs desc))))

(:const core::lambda-closure (:lambda (desc) (mu:sv-ref desc 4)))
(:const core::lambda-env (:lambda (desc) (mu:sv-ref desc 3)))
(:const core::lambda-macrop (:lambda (desc) (mu:sv-ref desc 2)))
(:const core::lambda-nreqs (:lambda (desc) (mu:length (core::lambda-reqs desc))))
(:const core::lambda-reqs (:lambda (desc) (mu:sv-ref desc 0)))
(:const core::lambda-rest (:lambda (desc) (mu:sv-ref desc 1)))
---

--- compile-lambda
(:const core::core-lambda
   (:lambda (form env)
      (:if (core:find-if (:lambda (el) (core:null (core:symbolp el))) form)
         (core:error form "core::core-lambda list syntax")
         ((:lambda (desc)
             (:if (core:fixnump desc)
                  (core::lambda-descriptor form () () env ())
                  desc))
          (core:foldl ;;; parse the lambda list
            (:lambda (el acc)
               (:if (core:vectorp acc)
                  acc
                  (:if (mu:eq '&rest el)
                     (:if (mu:eq (mu:length form) (mu:fixnum+ 2 acc))
                        (core::lambda-descriptor
                          (core:dropr form 2)
                          (mu:nth (mu:fixnum- (mu:length form) 1) form)
                          ()
                          env
                          ())
                        (core:error form "core::core-lambda rest botch"))
                     (mu:fixnum+ 1 acc))))
            0
            form)))))

(:const core::compile-lambda
   (:lambda (form env)
      ((:lambda (lambda-desc body)
          ((:lambda (fn-desc)
               (core::clone (mu:car fn-desc) 'lambda lambda-desc (mu:cdr fn-desc)))
           (core::compile-lambda-body
             (mu::compile (mu:cons :lambda (mu:cons (core::lambda-syms lambda-desc) ())))
             body
             env)))
       (core::core-lambda (mu:car form) env)
       (mu:nth 1 form))))
---

@s Compiling a lambda body
<hr><br/>
<p></p>
`core::compile-lambda-body` resolves body symbols with `core::compile-symbol`, uses the runtime compiler
for everything else, and returns a *function-descriptor*. If a body returns a function, a closure
may be generated.

<br/><br/>
<p></p>

[*function-descriptor*]
<br/>
>
> `(fn . compiled-body)`
>
<p></p>

[*closure-descriptor*]
<br/>
>
> `(function . values)`
>
<p></p>

`arguments:`
>
> `fn`: function
>
> `body:` list of body forms
>
> `env:` runtime lexical env
>
`returns:` *function-descriptor*

<br/><hr>

--- compile-closure --- noWeave
(:const core::closure
   (:lambda (fn)
      (core:assert core:functionp fn "core::closure requires a function")
      (core:assert core::fn-unclosedp fn "core::closure requires an unclosed lambda")
      ((:lambda (desc)
          (core::clone
            fn
            'closure
            (core::lambda-descriptor
              (core::lambda-reqs desc)
              (core::lambda-rest desc)
              (core::lambda-macrop desc)
              (core::lambda-env desc)
              (core:mapcar (:lambda (desc)
                            (mu::fr-get (mu:car desc)))
                            (core::lambda-env desc)))
            (mu:cdr (core::fn-form fn))))
       (core::fn-lambda fn))))
      
(:const core::compile-closure
   (:lambda (form env)
      (core:assert core:consp form "core::compile-closure requires a list")
      (core:maplist
        (:lambda (form-cons)
           (:if (mu:cdr form-cons)
              (mu:car form-cons)
              ((:lambda ()
                  (:if (core:functionp (mu:car form-cons))
                     ((:lambda (fn)
                         (:if (core::fn-unclosedp fn)
                              (core:compile (mu:cons 'core::closure (mu:cons fn ())) ())
                              fn))
                      (mu:car form-cons))
                     (mu:car form-cons))))))
        (core:mapcar (:lambda (form) (core:compile form env)) form))))
---

--- compile-lambda-body
(:const core::compile-lambda-body
   (:lambda (fn body env)
      (:if body
           ((:lambda (env)
               (mu:cons fn (core::compile-closure body env)))
            (core::compile-add-env fn env))
           (mu::compile '((:lambda ()))))))
---

@s Compiling symbols
<hr><br/>
<p></p>
`core::compile-symbol` looks up unqualified symbols in the current lexical environment. If found,
it compiles a *frame-reference* from the *lexical-descriptor* returned by `core::symbol-frame`.

<br/>
<p></p>

[*lexical-descriptor*]
<br/>
>
> `(lex-ref frame-id offset)`
>
<p></p>
<br/>
We have two sources of lexical bindings: from the *core* lambda being compiled and the
runtime's current lexical environment.

<br/>
<p></p>

`arguments:`
>
> `symbol:` symbol to be compiled
>
> `env:` compiled environment

`returns:` qualified symbol or *frame-ref*

<br/><hr>

--- compile-symbol
(:const core::compile-symbol
   (:lambda (symbol env)
      ((:lambda (env-ref)
          (:if env-ref
               (core:compile
                 (mu:cons 'mu::lex-ref (mu:cons (mu:car env-ref) (mu:cons (mu:cdr env-ref) ())))
                 env)
               (mu::compile symbol)))
       (core::symbol-frame symbol env))))
---

@s Resolving body symbols
<hr>
<p></p>
*symbol-frame* looks up *symbol* in a compiled environment.
If found return a *lexical-descriptor*, otherwise *nil*.
<br/><br/>
<p></p>

[*lexical-descriptor*]
<br/>
>
> `(frame-id . offset)`
>
<p></p>

`arguments:`
>
> `symbol:` symbol to be compiled
>
> `env:` lexical compiled environment

`returns:` *frame-ref* or *nil*

--- symbol-frame
(:const core::symbol-frame
   (:lambda (symbol env)
      (core:foldl
        (:lambda (frame acc)
           (:if acc
                acc
                ((:lambda (offset)
                    (:if offset
                         (mu:cons (core::frame-id frame) offset)
                         ()))
                 (core:assoc symbol (core::frame-symbols frame)))))
        ()
        env)))
---

--- core-lambda --- noWeave
@{compile-closure}
@{compile-lambda-body}
@{compile-lambda}
@{compile-symbol}
@{lambda-desc}
@{symbol-frame}
---

--- core:lambda.l --- noWeave
@{core-lambda}
---