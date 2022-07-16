;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;;  core namespace
;;;
(env:ns (mu:ns "core" (env:find-ns "mu")))
(mu::intern (env:find-ns "core") :intern "ns" (env:find-ns "core"))

;;; predicates
(mu::intern core::ns :extern "charp" (:lambda (t) (mu:eq :char (mu:type-of t))))
(mu::intern core::ns :extern "consp" (:lambda (t) (mu:eq :cons (mu:type-of t))))
(mu::intern core::ns :extern "doublep" (:lambda (t) (mu:eq :double (mu:type-of t))))
(mu::intern core::ns :extern "exceptionp" (:lambda (t) (mu:eq :except (mu:type-of t))))
(mu::intern core::ns :extern "fixnump" (:lambda (t) (mu:eq :fixnum (mu:type-of t))))
(mu::intern core::ns :extern "floatp" (:lambda (t) (mu:eq :float (mu:type-of t))))
(mu::intern core::ns :extern "functionp" (:lambda (t) (mu:eq :func (mu:type-of t))))
(mu::intern core::ns :extern "namespacep" (:lambda (t) (mu:eq :ns (mu:type-of t))))
(mu::intern core::ns :extern "null" (:lambda (t) (mu:eq :nil t)))
(mu::intern core::ns :extern "streamp" (:lambda (t) (mu:eq :stream (mu:type-of t))))
(mu::intern core::ns :extern "symbolp" (:lambda (t) (mu:eq :symbol (mu:type-of t))))
(mu::intern core::ns :extern "vectorp" (:lambda (t) (mu:eq :vector (mu:type-of t))))
(mu::intern core::ns :extern "listp" (:lambda (t) (:if (core:consp t) :t (core:null t))))
(mu::intern core::ns :extern "keywordp" (:lambda (t) (:if (mu:keysymp t) :t (core:null t))))
(mu::intern core::ns :extern "sequencep" (:lambda (t) (:if (core:listp t) :t (core:vectorp t))))

(mu::intern core::ns :extern "stringp"
   (:lambda (v)
      (:if (core:vectorp v)
         (mu:eq :char (mu:sv-type v))
         ())))

(mu::intern core::ns :extern "not" core:null)
(mu::intern core::ns :extern "zerop" (:lambda (t) (mu:eq 0 t)))

;;;
;;; symbols
;;;
(mu::intern core::ns :extern "symbol-name" mu:sy-name)
(mu::intern core::ns :extern "symbol-ns" mu:sy-ns)
(mu::intern core::ns :extern "symbol-value" mu:sy-val)
(mu::intern core::ns :extern "uninternedp"
   (:lambda (sym)
     (core:null (mu:sy-ns sym))))

;;; utilities
(mu::intern core::ns :extern "error"
   (:lambda (obj msg)
     (core:write "error: " mu:err-out ())
     (core:write msg mu:err-out ())
     (core:write " " mu:err-out ())
     (core:write obj mu:err-out :t)
     (core:terpri mu:err-out)
     (mu:raise (mu:except :co-err :error :cond obj))))

(mu::intern core::ns :extern "error-if"
   (:lambda (test msg)
     (:if test
          ((:lambda ()
             (core:write "error: " mu:err-out ())
             (core:write msg mu:err-out ())
             (core:write " " mu:err-out ())
             (core:write obj mu:err-out :t)
             (core:terpri mu:err-out)
             (mu:raise (mu:except :co-err :error :cond obj))))
          obj)))

(mu::intern core::ns :extern "errorp-when"
   (:lambda (predicate obj msg)
     (:if (predicate obj)
          ((:lambda ()
             (core:write "error: " mu:err-out ())
             (core:write msg mu:err-out ())
             (core:write " " mu:err-out ())
             (core:write obj mu:err-out ())
             (core:terpri mu:err-out)
             (mu:raise (mu:except :co-err :error :cond obj))))
          obj)))

(mu::intern core::ns :extern "errorp-unless"
   (:lambda (predicate obj msg)
     (:if (predicate obj)
          obj
          ((:lambda ()
             (core:write "error: " mu:err-out ())
             (core:write msg mu:err-out ())
             (core:write " " mu:err-out ())
             (core:write obj mu:err-out ())
             (core:terpri mu:err-out)
             (mu:raise (mu:except :co-err :error :cond obj)))))))

(mu::intern core::ns :extern "warn"
   (:lambda (obj msg)
     (core:write "warn: " mu:err-out ())
     (core:write msg mu:err-out ())
     (core:write " " mu:err-out ())
     (core:write obj mu:err-out :t)
     (core:terpri mu:err-out)
     obj))

(mu::intern core::ns :extern "1+"
   (:lambda (n)
      (core:errorp-unless core:fixnump n "core:1+: not a fixnum")
      (mu:fixnum+ n 1)))

(mu::intern core::ns :extern "1-"
   (:lambda (n)
      (core:errorp-unless core:fixnump n "core:1-: not a fixnum")
     (mu:fixnum- n 1)))

