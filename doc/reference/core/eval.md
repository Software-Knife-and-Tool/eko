---
title: print library
---

@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Evaluator

@s Summary
<hr><br/>
<p></p>
The *core* evaluator defers *mu* forms to the runtime evaluator. Core forms with rest
arguments are massaged into a fixed-arity function call that can be executed by the
runtime.
<br/>

--- core predicates --- noWeave
(:const core:doublep (:lambda (t) (mu:eq :double (mu:type-of t))))
(:const core:exceptionp (:lambda (t) (mu:eq :except (mu:type-of t))))
(:const core:fixnump (:lambda (t) (mu:eq :fixnum (mu:type-of t))))
(:const core:floatp (:lambda (t) (mu:eq :float (mu:type-of t))))
(:const core:functionp (:lambda (t) (mu:eq :func (mu:type-of t))))
(:const core:namespacep (:lambda (t) (mu:eq :ns (mu:type-of t))))

(:const core:symbolp (:lambda (t) (mu:eq :symbol (mu:type-of t))))
(:const core:vectorp (:lambda (t) (mu:eq :vector (mu:type-of t))))
(:const core:zerop (:lambda (t) (mu:eq 0 t)))
(:const core:not (:lambda (t) (mu:eq :nil t)))
---

@s Function Calls
<hr><br/>
<p></p>
*core::fn-call* is used internally by the *core* compiler to wrap a *core lambda* function call
so that it can be executed by the rutime in a *mu* lambda body.

<br/>
<p></p>
*core:funcall* converts a *core* rest form function call into a fixed-arity form for consumption
by the runtime. *mu* function calls are executed directly by the runtime.
<br/>

--- fn-call
(:const core::fn-call-closure
   (:lambda (fn args)
      ((:lambda (desc)
          (core:assert core:identity desc "core::fn-call-closure requires a lambda descriptor")
          ((:lambda (closure)
              (:if closure
                   ((:lambda ()
                       (core:mapc mu::fr-push closure)
                       ((:lambda (value)
                           (core:mapc (:lambda (closure-desc) (mu::fr-pop (mu:car closure-desc))) closure)
                           value)
                        (mu:funcall fn args))))
                   (mu:funcall fn args)))
           (core::lambda-closure desc)))
        (core::fn-lambda fn))))

(:const core::fn-call
   (:lambda (fn args)
      (core:funcall fn (core:mapcar core:eval args))))
      
(:const core:funcall
   (:lambda (fn args)
      (core:assert core:functionp fn "core:funcall requires a function")
      (core:assert core:listp args "core:funcall requires a list")
      (:if (core::fn-lambda fn)
         (:if (core::fn-restp fn)
            ((:lambda (nreqs nargs)
                ((:lambda (reqs rest)
                    ((:lambda (args) (core::fn-call-closure fn args))
                     (core::append reqs (mu:cons rest ()))))
                 (core:dropr args (mu:fixnum- nargs nreqs))
                 (core:dropl args nreqs)))
             (core::fn-nreqs fn)
             (mu:length args))
            (core::fn-call-closure fn args))
         (mu:funcall fn args))))
---

@s Eval
<hr><br/>
<p></p>
*core:eval* performs the usual evaluator responsibilities, preparing function calls
for consumption by *core:function*, dereferenixng symbols, and deferring evaluation for
quoted forms. *mu* forms are handled by the runtime evaluator.

--- eval
(:const core:eval
   (:lambda (form)
      (:if (core:consp form)
           ((:lambda (fn args)
               (:if (core:functionp fn)
                    (core:funcall fn (core:mapcar core:eval args))
                    (:if (core:symbolp fn)
                         (:if (mu:eq :quote fn)
                              (mu:car args)
                              (core:funcall (mu:sy-val fn) (core:mapcar core:eval args)))
                         (core:error fn "core:eval requires a function designator"))))
            (mu:car form)
            (mu:cdr form))
           (mu:eval form))))
---

--- core-eval --- noWeave
@{core predicates}
@{fn-call}
@{eval}
---

--- core:eval.l --- noWeave
@{core-eval}
---
