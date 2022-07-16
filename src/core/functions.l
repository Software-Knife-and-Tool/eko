;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; clones
;;;
(mu::intern core::ns :intern "clone-function"
  (:lambda (fn lambda-desc body)
     ;;; (core:errorp-unless core:functionp fn "clone: not a function")      
     ;;; (core:errorp-unless core:vectorp lambda-desc "clone: not a vector")
     ;;; (core:errorp-unless core:listp body "clone: not a list")
     (mu::fclone
       (mu:list-sv :t
         (mu:cons (mu:fixnum+ (core::lambda-nreqs lambda-desc)
                              (:if (core::lambda-rest-symbol lambda-desc) 1 0))
           (mu:cons (mu:cons (core::lambda-syms lambda-desc) body)
             (mu:cons (core::fn-frame-id fn)
               (mu:cons lambda-desc ()))))))))

;;;
;;; accessors/predicates
;;;
(mu::intern core::ns :intern "fn-arity" (:lambda (fn) (mu::fn-int :arity fn)))
(mu::intern core::ns :intern "fn-form" (:lambda (fn) (mu::fn-int :form fn)))
(mu::intern core::ns :intern "fn-frame-id" (:lambda (fn) (mu::fn-int :frame fn)))
(mu::intern core::ns :intern "fn-extension" (:lambda (fn) (mu::fn-int :ext fn)))
(mu::intern core::ns :intern "fn-nreqs"
  (:lambda (fn)
     ((:lambda (desc)
         (:if desc
              (core::lambda-nreqs desc)
              (core::fn-arity fn)))
      (core::fn-extension fn))))

(mu::intern core::ns :intern "fn-unclosedp"
   (:lambda (fn)
      ((:lambda (desc)
          (:if (core::lambda-env desc)
             (core:null (core::lambda-closure desc))
             ()))
       (core::fn-extension fn))))

(mu::intern core::ns :intern "fn-macrop"
   (:lambda (fn)
     ((:lambda (desc)
       (:if desc
            (core::lambda-macrop desc)
            ()))
      (core::fn-extension fn))))

(mu::intern core::ns :intern "fn-restp"
  (:lambda (fn)
     ((:lambda (desc)
         (:if desc
              (core::lambda-rest-symbol desc)
              ()))
      (core::fn-extension fn))))

