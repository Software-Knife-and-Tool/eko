;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; macro definition, compilation, and expansion
;;;

;;;
;;; Macro definitions are compiled like lambdas (though eventually with a slighlty
;;; different lambda list including &whole).
;;;
;;; Macro calls are expanded at compile time.
;;;
;;; core::macroexpand and core::macro-expand-1 expand a macro definition against a form.
;;;
;;; (Common Lisp adds an environment argument here, think about why.)
;;;

;;;
;;; core::macroexpand-1 expands the form just once.
;;;
(mu::intern core::ns :intern "macroexpand-1"
   (:lambda (form)
      (:if (core:consp form)
         ((:lambda (fn args)
             (:if (mu:keysymp fn)
                form
                (:if (core::special-table fn)
                   form
                   (:if (core:macro-function fn)
                        (core:funcall (core:macro-function fn) args)
                        form))))
          (mu:car form)
          (mu:cdr form))
        form)))

;;;
;;; core:macroexpand: expand a form until it no longer is a macro call
;;;
(mu::intern core::ns :extern "macroexpand"
   (:lambda (form)
      ((:lambda (form)
          (:if (mu:eq form (core::macroexpand-1 form))
             form
             (core:macroexpand form)))
       (core::macroexpand-1 form))))

;;;
;;; core::macro-function extracts the macro function from a function or symbol
;;;
;;; acts as a predicate
;;;
(mu::intern core::ns :extern "macro-function"
   (:lambda (fn)
      (:if (core:functionp fn)
         (:if (core::fn-macrop fn) fn ())
         (:if (core:symbolp fn)
            (:if (mu:keysymp fn)
               ()
               (core:macro-function (mu:sy-val fn)))
            ()))))

;;;
;;; core::compile-macro form env => function
;;;
;;; core::compile-macro compiles a macro expander function.
;;;
(mu::intern core::ns :intern "core-macro"
   (:lambda (form env)
      (:if (core:findl-if (:lambda (el) (core:null (core:symbolp el))) form)
         (core:error-if form "core::core-macro lambda syntax")
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
                          (mu:nth (core:1- (mu:length form)) form)
                          :t
                          env
                          ())
                        (core:error form "core-lambda rest botch"))
                     (core:1+ acc))))
            0
            form)))))

(mu::intern core::ns :intern "compile-macro"
   (:lambda (form env)
      (:if form
         ((:lambda (lambda-desc body)
             ((:lambda (fn-desc)
                 (core::clone-function
                   (mu:car fn-desc)
                   lambda-desc
                   (mu:cdr fn-desc)))
              (core::compile-lambda-body
                (mu::compile
                  (mu:cons :lambda (mu:cons (core::lambda-syms lambda-desc) ())))
                body
                env)))
          (core::core-macro (mu:car form) env)
          (mu:nthcdr 1 form))
         (core:error form "core::compile-macro lambda arity"))))

