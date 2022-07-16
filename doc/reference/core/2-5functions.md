---
title: print library
---

0@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Functions

@s Summary
<hr><br/>
<p></p>
We'll need some utilities to supplement the code generation functions in *core.l*.

@s Function Clones
<hr><br/>
<p></p>

Function clones.

--- function clones
(:const core::clone
  (:lambda (fn name lambda-desc body)
     (core:check-type fn :func "core::clone requires a function")
     (core:check-type name :symbol "core::clone requires a symbol")
     (core:check-type lambda-desc :vector "core::clone requires a lambda descriptor")
     (core:assert core:listp body "core::clone requires a list")
     ((:lambda (restp)
         (mu::clone
           (mu:list-sv :t
             (mu:cons (mu:fixnum+ (core::lambda-nreqs lambda-desc) (:if restp 1 0))
               (mu:cons (mu:cons (core::lambda-syms lambda-desc) body)
                 (mu:cons (core::fn-frame-id fn)
                   (mu:cons name
                     (mu:cons lambda-desc ()))))))))
         (core::lambda-rest lambda-desc))))
---

@s Function Accessors
<hr><br/>
<p></p>

Function implementation.

--- function accessors
(:const core::fn-arity (:lambda (fn) (mu::fn-acsr :arity fn)))
(:const core::fn-form (:lambda (fn) (mu::fn-acsr :form fn)))
(:const core::fn-frame-id (:lambda (fn) (mu::fn-acsr :frame fn)))
(:const core::fn-lambda (:lambda (fn) (mu::fn-acsr :ext fn)))
(:const core::fn-name (:lambda (fn) (mu::fn-acsr :name fn)))
(:const core::fn-nreqs
  (:lambda (fn)
     ((:lambda (lambda-desc)
         (:if (core:null lambda-desc)
              (core::fn-arity fn)
              (core::lambda-nreqs lambda-desc)))
      (core::fn-lambda fn))))

(:const core::fn-unclosedp
   (:lambda (fn)
      ((:lambda (ext)
          (:if (core::lambda-env ext)
             (core:null (core::lambda-closure ext))
             ()))
       (core::fn-lambda fn))))

(:const core::fn-macrop
  (:lambda (fn)
    (:if (core::fn-lambda fn)
         (mu:sv-ref (core::fn-lambda fn) 2)
         ())))

(:const core::fn-restp
  (:lambda (fn)
     ((:lambda (lambda-desc)
         (:if (core:null lambda-desc)
              ()
              (core::lambda-rest lambda-desc)))
      (core::fn-lambda fn))))

---

--- core-functions --- noWeave
@{function clones}
@{function accessors}
---

--- core:functions.l --- noWeave
@{core-functions}
---
