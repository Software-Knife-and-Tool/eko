;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; core lambdas
;;;

;;;
;;; [*lambda descriptor*] #(:t req-syms rest-sym body macrop env closure)
;;;
;;; req-syms: list of required symbols, from lambda
;;; rest- sym: rest symbol | (), from lambda
;;; macrop: () | :t
;;; env: closed symbols, list of functions
;;; closure: closed symbol state, list of bindings
;;;
(mu::intern core::ns :intern "lambda-descriptor"
   (:lambda (req-syms rest-sym macrop env closure)
      (mu:list-sv :t
        (mu:cons req-syms
          (mu:cons rest-sym
            (mu:cons macrop
              (mu:cons env
                (mu:cons closure ()))))))))

(mu::intern core::ns :intern "lambda-syms"
  (:lambda (desc)
     (:if (core::lambda-rest-symbol desc)
       (core::append (core::lambda-req-symbols desc) (mu:cons (core::lambda-rest-symbol desc) ()))
       (core::lambda-req-symbols desc))))

(mu::intern core::ns :intern "lambda-closure" (:lambda (desc) (mu:sv-ref desc 4)))
(mu::intern core::ns :intern "lambda-env" (:lambda (desc) (mu:sv-ref desc 3)))
(mu::intern core::ns :intern "lambda-macrop" (:lambda (desc) (mu:sv-ref desc 2)))
(mu::intern core::ns :intern "lambda-nreqs" (:lambda (desc) (mu:length (core::lambda-req-symbols desc))))
(mu::intern core::ns :intern "lambda-req-symbols" (:lambda (desc) (mu:sv-ref desc 0)))
(mu::intern core::ns :intern "lambda-rest-symbol" (:lambda (desc) (mu:sv-ref desc 1)))

(mu::intern core::ns :intern "core-lambda"
   (:lambda (form env)
      (:if (core:findl-if (:lambda (el) (core:null (core:symbolp el))) form)
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
                          (mu:nth (core:1- (mu:length form)) form)
                          ()
                          env
                          ())
                        (core:error form "core::core-lambda rest botch"))
                     (core:1+ acc))))
            0
            form)))))

(mu::intern core::ns :intern "compile-lambda"
   (:lambda (form env)
      ((:lambda (lambda-desc body)
          ((:lambda (fn-desc)
               (core::clone-function (mu:car fn-desc) lambda-desc (mu:cdr fn-desc)))
           (core::compile-lambda-body
             (mu::compile (mu:cons :lambda (mu:cons (core::lambda-syms lambda-desc) ())))
             body
             env)))
       (core::core-lambda (mu:car form) env)
       (mu:nth 1 form))))

(mu::intern core::ns :intern "closure"
   (:lambda (fn)
      ; (core:errorp-unless core:functionp fn "closure: not a function")
      ; (core:errorp-unless core::fn-unclosedp fn "closure: already closed")
      ((:lambda (desc)
          (core::clone-function
            fn
            (core::lambda-descriptor
              (core::lambda-req-symbols desc)
              (core::lambda-rest-symbol desc)
              (core::lambda-macrop desc)
              (core::lambda-env desc)
              (core:mapcar
                (:lambda (desc) (env:fnv-lex (mu:car desc)))
                (core::lambda-env desc)))
            (mu:cdr (core::fn-form fn))))
       (core::fn-extension fn))))

;;;
;;; [*closure-descriptor*] `(function . values)` 
;;;      
(mu::intern core::ns :intern "compile-closure"
   (:lambda (form env)
      ; (core:errorp-unless core:consp form "compile: not a cons")
      (core:maplist
        (:lambda (form-cons)
           (:if (mu:cdr form-cons)
              (mu:car form-cons)
              (:if (core:functionp (mu:car form-cons))
                   ((:lambda (fn)
                       (:if (core::fn-unclosedp fn)
                            (core:compile (mu:cons 'core::closure (mu:cons fn ())))
                            fn))
                    (mu:car form-cons))
                   (mu:car form-cons))))
        (core:mapcar (:lambda (form) (core::compile form env)) form))))

;;;
;;; `core::compile-lambda-body` resolves body symbols with `core::compile-symbol`, uses the runtime compiler
;;; for everything else, and returns a *function-descriptor*. If a body returns a function, a closure
;;; will be generated.
;;;
;;; [*function-descriptor*] `(fn . compiled-body)`
;;;
;;; `fn`: function
;;; `body:` list of body forms
;;; `env:` runtime lexical env
;;;
;;; `returns:` *function-descriptor*
;;;
(mu::intern core::ns :intern "compile-lambda-body"
   (:lambda (fn body env)
      (:if body
           ((:lambda (env)
               (mu:cons fn (core::compile-closure body env)))
            (core::compile-add-env fn env))
           (mu::compile '((:lambda ()))))))


;;;
;;; Compiling symbols
;;;
;;; `core::compile-symbol` looks up unqualified symbols in the current lexical environment. If found,
;;; it compiles a *frame-reference* from the *lexical-descriptor* returned by `core::symbol-frame`.
;;;
;;; [*lexical-descriptor*] `(lex-ref frame-id offset)`
;;;
;;; We have two sources of lexical bindings: from the *core* lambda being compiled and the
;;; runtime's current lexical environment.
;;;
;;;
;;; `symbol:` symbol to be compiled
;;;
;;;  `env:` compiled environment
;;;
;;; returns:` qualified symbol or *frame-ref*
;;;
(mu::intern core::ns :intern "compile-symbol"
   (:lambda (symbol env)
      ((:lambda (env-ref)
          (:if env-ref
               (core::compile
                 (mu:cons 'env:lex-ref (mu:cons (mu:car env-ref) (mu:cons (mu:cdr env-ref) ())))
                 env)
               (mu::compile symbol)))
       (core::symbol-frame symbol env))))

;;;
;;;  Resolving body symbols
;;;
;;; *symbol-frame* looks up *symbol* in a compiled environment.
;;;
;;; if found return a *lexical-descriptor*, otherwise *nil*.
;;;
;;; [*lexical-descriptor*] `(frame-id . offset)`
;;;
;;; `symbol:` symbol to be compiled
;;;
;;; `env:` lexical compiled environment
;;;
;;; `returns:` *frame-ref* or *nil*
;;;
(mu::intern core::ns :intern "symbol-frame"
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

