;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; compile.l: core compiler
;;;
;;; compiling an environment
;;; 
;;;    core::compile-add-env function env => list
;;;
;;; core::complle-add-env converts a function to a frame-descriptor
;;; and adds it to a list of frame-descriptors.
;;;
;;;    frame-descriptor :: (frame-id . ((symbol . offset)...))
;;;
;;; arguments:
;;;
;;;   fn  : function
;;;   env : lexical environment list
;;;
;;; returns: augmented list of frame-descriptors
;;;
(mu::intern core::ns :intern "frame-descriptor"
   (:lambda (fn symbols)
      (mu:cons fn symbols)))

(mu::intern core::ns :intern "frame-fn" (:lambda (desc) (mu:car desc)))
(mu::intern core::ns :intern "frame-id" (:lambda (desc) (core::fn-frame-id (core::frame-fn desc))))
(mu::intern core::ns :intern "frame-symbols" (:lambda (desc) (mu:cdr desc)))

(mu::intern core::ns :intern "compile-add-env"
  (:lambda (fn env)
     ; (core:errorp-unless core:functionp fn "add-env: not a function")
     ; (core:errorp-unless core:listp env "add-env: not a list")  
     (mu:cons
       (core::frame-descriptor
         fn
         (core:foldl
           (:lambda (sym acc) (mu:cons (mu:cons sym (mu:length acc)) acc))
           ()
           (mu:car (core::fn-form fn))))
       env)))

;;;
;;; compiling special/macro forms
;;; 
;;;    core::defconst form env => list
;;;    core::defmacro form env => list
;;;    core::defun form env => list
;;;    core::if form env => list
;;;    core::lambda form env => list
;;;    core::quote form env => list
;;;
;;; special/macro form compiler rewrites a form against
;;; an environment into something the runtime can execute.
;;;
;;; arguments:
;;;
;;;   form : any form
;;;   env  : lexical environment list
;;;
;;; returns: mu form
;;;
(mu::intern core::ns :intern "def-symbol"
  (:lambda (symbol value env)
    (:if (core:uninternedp symbol)
         ((:lambda (sym)
            (:if sym
                 (:if (mu:boundp sym)
                      (core:error sym "def-symbol: symbol already bound")
                      (mu::intern
                       (core::read-get-ns)
                       :extern
                       (mu:sy-name symbol)
                       value))
                 (mu::intern
                  (core::read-get-ns)
                  :extern
                  (mu:sy-name symbol)
                  value)))
          (mu:ns-find (core::read-get-ns) :extern (mu:sy-name symbol)))
         ((:lambda (ns name)
             (mu::intern
              (core::read-get-ns)
              (:if (mu:ns-find ns :extern name)
                   :extern
                   :intern)
              name
              value))
           (mu:sy-ns symbol)
           (mu:sy-name symbol)))))

(mu::intern core::ns :intern "quote" (:lambda (form env) form))

(mu::intern core::ns :intern "lambda"
   (:lambda (form env)
      (core::compile-lambda
        (mu:cons (mu:car (mu:cdr form)) (mu:cons (mu:cdr (mu:cdr form)) ()))
        env)))

(mu::intern core::ns :intern "defconst"
   (:lambda (form env)
     ((:lambda (symbol value)
        (core:errorp-unless core:symbolp symbol "core:defconst not a symbol")
        (core:errorp-when core:keywordp symbol "core:defconst not a symbol")
        (mu::compile (mu:cons :quote
                       (mu:cons (core::def-symbol
                                    symbol
                                    (core:eval (core::compile value env))
                                  env)
                         ()))))
      (mu:nth 1 form)
      (mu:nth 2 form))))

(mu::intern core::ns :intern "defmacro"
   (:lambda (form env)
      ((:lambda (symbol lambda)
         (core:errorp-unless core:symbolp symbol "core::defmacro not a symbol")
         (core:errorp-when core:keywordp symbol "core::defmacro not a symbol")
         (core:errorp-unless core:listp lambda "core::defmacro not a list")
         (mu::compile (mu:cons :quote
                        (mu:cons (core::def-symbol symbol (core::compile-macro lambda env) env)
                          ()))))
       (mu:nth 1 form)
       (mu:nthcdr 2 form))))

(mu::intern core::ns :intern "defun"
   (:lambda (form env)
      ((:lambda (symbol lambda)
         (core:errorp-unless core:symbolp symbol "core::defun: not a symbol")
         (core:errorp-when core:keywordp symbol "core::defun: not a symbol")
         (core:errorp-unless core:listp lambda "core::defun: not a lambda list")
         (mu::compile (mu:cons :quote
                        (mu:cons (core::def-symbol symbol (core::lambda (mu:cons :lambda lambda) env) env)
                          ()))))
       (mu:nth 1 form)
       (mu:nthcdr 2 form))))

;;;
;;; this if form can take two or three arguments
;;;
(mu::intern core::ns :intern "if"
   (:lambda (form env)
      (core:errorp-unless core:listp env "core::if not an environment")
      (core:errorp-unless (:lambda (form) (mu:fixnum< 2 (mu:length form))) form "core:if takes two or three arguments")
      (core:errorp-unless (:lambda (form) (mu:fixnum< (mu:length form) 5)) form "core:if takes two or three arguments")
      (core::compile
        ((:lambda (thunk if-impl)
            (mu:cons 'core:funcall
              (mu:cons (thunk
                        (mu:cons if-impl
                          (mu:cons (core::compile (mu:nth 1 form) env)
                            (mu:cons (thunk (mu:nth 2 form))
                              (mu:cons (thunk (mu:nth 3 form))
                                ())))))
                '(()))))
         (:lambda (form)
            (core::compile
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

(mu::intern core::ns :intern "special-table"
   (:lambda (symbol)
      (core:assoc
        symbol
        '((defconst    . core::defconst)
          (defun       . core::defun)
          (defmacro    . core::defmacro)
          (if          . core::if)
          (lambda      . core::lambda)
          (:quote      . core::quote)))))

;;;
;;; [*core lambda form*]
;;; (lambda ([symbol ...] [&rest rest-symbol]) . body)
;;;
(mu::intern core::ns :intern "must-funcall"
  (:lambda (fn env)
     (:if (core::fn-extension fn)
          (:if (core::fn-restp fn)
               :t
               (:if (core:symbolp fn)
                    ((:lambda (sym))
                        (core:null (core:symbolp sym))
                        (core::compile-symbol fn env))
                    (core:null (core::fn-unclosedp fn))))
          ())))

(mu::intern core::ns :intern "make-funcall"
   (:lambda (fn args env)
      (mu:cons 'core::funcall
        (mu:cons fn
          (mu:cons
            (mu:cons :quote
              (mu:cons (core:mapcar (:lambda (el) (core::compile el env)) args)
                ()))
            ())))))

(mu::intern core::ns :intern "compile-activation"
   (:lambda (designator args env)
      ((:lambda (fn)
          (:if (core:functionp fn)
               (:if (core::fn-macrop fn)
                    (core::compile (core:macroexpand (mu:cons fn args)) env)                     
                    (:if (core::must-funcall fn env)
                         (core::make-funcall fn args env)
                         (mu:cons fn (core:mapcar (:lambda (el) (core::compile el env)) args))))
               (core::make-funcall (core::compile-symbol fn env) args env)))
       (:if (core:functionp designator)
            designator
            (:if (core:symbolp designator)
                 (:if (mu:boundp designator)
                      (mu:sy-val designator)
                      designator)
                 (core:error-if designator "core::compile-activation: not a function designator"))))))

(mu::intern core::ns :intern "compile"
   (:lambda (form env)
      (core:errorp-unless core:listp env "core::compile not a list")
      (:if (core:consp form)
           ((:lambda (fn args)
               (:if (mu:keysymp fn)
                    (mu::compile form)    ;;; libmu special form (:key ...)
                    (:if (core:consp fn)  ;;; implied application ((...) ...), native or core
                         (core:mapcar (:lambda (el) (core::compile el env)) form)
                         ((:lambda (specf)
                             (:if specf
                                  (core:funcall (mu:sy-val specf) (mu:cons form (mu:cons env ())))
                                  (core::compile-activation fn args env)))
                          (core::special-table fn)))))
            (mu:car form)
            (mu:cdr form))
           (:if (core:symbolp form)
                (core::compile-symbol form env)
                (mu::compile form)))))

(mu::intern core::ns :extern "compile"
   (:lambda (form)
     (core::compile form ())))

