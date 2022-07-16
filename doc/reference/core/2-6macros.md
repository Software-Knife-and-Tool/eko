---
title: print library
---

@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Macros

@s Summary
<hr><br/>
<p></p>
Macro definitions are compiled like lambdas (though eventually with a slighlty
different lambdaa list including &whole) and macro calls are expanded at compile time.
<br/>

@s Macro compilation and expansion
<hr>
<p></p>
***core::macroexpand-1*** *form* ***=>*** *form*
<p></p>
***core::macroexpand*** *form* ***=>*** *form*
<br/><br/>
`core::macroexpand` and `core::macro-expand-1` expand a macro definition against a form.
`core::macroexpand` expands a form until it no longer is a macro call, `core::macroexpand-1`
expands the form just once.
<br/><br/><p></p>
(Common Lisp adds an environment argument here, think about why.)

<br/><p></p>

`arguments:`
>
> `form:` whole form

`returns:` *uncompiled expanded form*

<hr>
<p></p>
***core::compile-macro*** *form* *env* ***=>*** *function*
<br/><br/>
`core::compile-macro` compiles a macro expander function.

<br/><p></p>

`arguments:`
>
> `form:` whole form
> `env;` environment

`returns:` *function*

--- compile-macros
(:const core::macroexpand-1
   (:lambda (form)
      (:if (core:consp form)
         ((:lambda (fn args)
             (:if (mu:keyp fn)
                form
                (:if (core::special fn)
                   form
                   (:if (core:macro-function fn)
                      ((:lambda ()
                         (core:funcall (core:macro-function fn) args)))
                      form))))
          (mu:car form)
          (mu:cdr form))
        form)))

(:const core:macroexpand
   (:lambda (form)
      ((:lambda (form)
          (:if (mu:eq form (core::macroexpand-1 form))
             form
             (core:macroexpand form)))
       (core::macroexpand-1 form))))

(:const core:macro-function
   (:lambda (fn)
      (:if (core:functionp fn)
         (:if (core::fn-macrop fn) fn ())
         (:if (core:symbolp fn)
            (:if (mu:keyp fn)
               ()
               (core:macro-function (mu:sy-val fn)))
            ()))))

(:const core::core-macro
   (:lambda (form env)
      (:if (core:find-if (:lambda (el) (core:null (core:symbolp el))) form)
         (core:error form "core::core-macro lambda syntax")
         ((:lambda (desc)
             (:if (core:fixnump desc)
                  (core::lambda-descriptor form () :t env ())
                  desc))
          (core:foldl
            (:lambda (el acc)
               (:if (core:vectorp acc)
                  acc
                  (:if (mu:eq '&rest el)
                     (:if (mu:eq (mu:length form) (mu:fixnum+ 2 acc))
                        (core::lambda-descriptor
                          (core:dropr form 2)
                          (mu:nth (mu:fixnum- (mu:length form) 1) form)
                          :t
                          env
                          ())
                        (core:error form "core-lambda rest botch"))
                     (mu:fixnum+ 1 acc))))
            0
            form)))))

(:const core::compile-macro
   (:lambda (form env)
      (:if (core:null form)
         (core:error form "core::compile-macro lambda arity")
         ((:lambda (lambda-desc body)
             ((:lambda (fn-desc)
                 (core::clone
                   (mu:car fn-desc)
                   'macro
                   lambda-desc
                   (mu:cdr fn-desc)))
              (core::compile-lambda-body
                (mu::compile
                  (mu:cons :lambda (mu:cons (core::lambda-syms lambda-desc) ())))
                body
                env)))
          (core::core-macro (mu:car form) env)
          (mu:nthcdr 1 form)))))
---

--- core-macro --- noWeave
@{compile-macros}
---

--- core:macro.l --- noWeave
@{core-macro}
---