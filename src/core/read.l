;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; core reader
;;;
(mu::intern core::ns :intern "reader-stream" (mu:open :string :output ""))
(mu::intern core::ns :intern "reader-ns-stream" (mu:open :string :output "core"))

;;;
;;; namespaces
;;;
(mu::intern core::ns :extern "in-namespace"
   (:lambda (ns)
     (core:errorp-unless core:namespacep ns "core:in-namespace: not a namespace")
     (env:ns ns)
     (core::read-set-ns ns)
     ns))
   
(mu::intern core::ns :intern "read-get-ns"
   (:lambda ()
     ((:lambda (name)
        (mu:write name core::reader-ns-stream ())
        (env:find-ns name))
      (mu:get-str core::reader-ns-stream))))

(mu::intern core::ns :intern "read-set-ns"
   (:lambda (ns)
      (mu:get-str core::reader-ns-stream)
      (mu:write (mu:ns-name ns) core::reader-ns-stream ())))

;;;
;;; readtable
;;;
(mu::intern core::ns :intern "read-whitespace"
    (mu:cons (mu:cons (mu:code-ch #xa) 'wspace)
      (mu:cons (mu:cons (mu:code-ch #xd) 'wspace)
        (mu:cons (mu:cons (mu:code-ch #xc) 'wspace)
          (mu:cons (mu:cons (mu:code-ch #x20) 'wspace)
            ())))))

(mu::intern core::ns :intern "read-char-syntax"
   (:lambda (ch)
     ((:lambda (read-table)
         (core:assoc ch read-table))
      (core::append
       core::read-whitespace      
       '((#\0 . constituent)  (#\1 . constituent)  (#\2 . constituent)  (#\3 . constituent)
         (#\4 . constituent)  (#\5 . constituent)  (#\6 . constituent)  (#\7 . constituent)
         (#\8 . constituent)  (#\9 . constituent)  (#\: . constituent)  (#\< . constituent)
         (#\> . constituent)  (#\= . constituent)  (#\? . constituent)  (#\! . constituent)
         (#\@ . constituent)  (#\; . tmacro)       (#\" . tmacro)       (#\# . macro)
         (#\' . tmacro)       (#\( . tmacro)       (#\) . tmacro)       (#\` . tmacro)
         (#\, . tmacro)       (#\\ . escape)       (#\| . mescape)      (#\a . constituent)
         (#\b . constituent)  (#\c . constituent)  (#\d . constituent)  (#\e . constituent)
         (#\f . constituent)  (#\g . constituent)  (#\h . constituent)  (#\i . constituent)
         (#\j . constituent)  (#\k . constituent)  (#\l . constituent)  (#\m . constituent)
         (#\n . constituent)  (#\o . constituent)  (#\p . constituent)  (#\q . constituent)
         (#\r . constituent)  (#\s . constituent)  (#\t . constituent)  (#\v . constituent)
         (#\w . constituent)  (#\x . constituent)  (#\y . constituent)  (#\z . constituent)
         (#\[ . constituent)  (#\] . constituent)  (#\$ . constituent)  (#\* . constituent)
         (#\{ . constituent)  (#\} . constituent)  (#\+ . constituent)  (#\- . constituent)
         (#\/ . constituent)  (#\~ . constituent)  (#\. . constituent)  (#\% . constituent)
         (#\& . constituent)  (#\^ . constituent)  (#\_ . constituent)  (#\a . constituent)
         (#\b . constituent)  (#\c . constituent)  (#\d . constituent)  (#\e . constituent)
         (#\f . constituent)  (#\g . constituent)  (#\h . constituent)  (#\i . constituent)
         (#\j . constituent)  (#\k . constituent)  (#\l . constituent)  (#\m . constituent)
         (#\n . constituent)  (#\o . constituent)  (#\p . constituent)  (#\q . constituent)
         (#\r . constituent)  (#\s . constituent)  (#\t . constituent)  (#\u . constituent)
         (#\v . constituent)  (#\w . constituent)  (#\x . constituent)  (#\y . constituent)
         (#\z . constituent))))))

;;;
;;; sharpsign readers
;;;
(mu::intern core::ns :intern "read-sharp-char"
   (:lambda (ch stream)
      (mu:rd-char stream)))

(mu::intern core::ns :intern "read-sharp-comment"
   (:lambda (ch stream)
      ((:lambda (loop) (loop loop (mu:rd-char stream)))
       (:lambda (loop ch)
          (:if (mu:eq ch #\|)
               ((:lambda (ch)
                   (:if (mu:eq ch #\#)
                        ()
                        (loop loop (mu:rd-char stream))))
                (mu:rd-char stream))
               (loop loop (mu:rd-char stream)))))))
      
(mu::intern core::ns :intern "read-sharp-vector"
   (:lambda (ch stream)
      ((:lambda (type)
          (core:errorp-unless mu:keysymp type "core:read-sharp-vector not a type keyword")
          (mu:list-sv type (core::read-list #\( stream)))
       (core::read-atom stream))))

(mu::intern core::ns :intern "read-sharp-symbol"
   (:lambda (ch stream)
      ((:lambda (symbol)
          (core:errorp-unless core:symbolp symbol "core::read-sharp-symbol not a symbol")
          (mu:sy-make (mu:symbol symbol)))
       (core::read-atom stream))))

(mu::intern core::ns :intern "read-sharp"
   (:lambda (ch stream)
      ((:lambda (ch sharp-table)
          (mu:funcall
            (mu:sy-val (core:assoc ch sharp-table))
            (mu:cons ch (mu:cons stream ()))))
       (mu:rd-char stream)
       '((#\| . core::read-sharp-comment)
         (#\\ . core::read-sharp-char)
         (#\( . core::read-sharp-vector)
         (#\: . core::read-sharp-symbol)))))

;;;
;;; symbols
;;;
(mu::intern core::ns :intern "read-symbol-keywordp"
   (:lambda (name)
      (mu:eq #\: (core:schar name 0))))

(mu::intern core::ns :intern "read-symbol-externp"
   (:lambda (name)
      (mu:eq (core:positionl #\: name) (core:positionr #\: name))))

(mu::intern core::ns :intern "read-symbol-ns"
   (:lambda (name)
      ((:lambda (colon)
          (:if (core:null colon)
               ()
               (core:substr name 0 (core:1- colon))))
       (core:positionl #\: name))))

(mu::intern core::ns :intern "read-symbol-name"
   (:lambda (name)
      ((:lambda (colon)
          (:if (core:null colon)
               name
               (core:substr
                 name
                 (core:1+ colon)
                 (core:1- (mu:sv-len name)))))
       (core:positionr #\: name))))

(mu::intern core::ns :intern "read-resolve-symbol"
   (:lambda (symbol)
      (:if (core::read-symbol-keywordp symbol)
           (mu:keyword (core::read-symbol-name symbol))
           ((:lambda (ns scope base)
;;;              (core:warn ns "read-resolve-sym: ns")
              (:if (core:null ns)
                   ((:lambda (sym) sym)
                    (:if (core:string= base "t")
                         :t
                         (:if (core:string= base "nil")
                              :nil
                              ((:lambda (sym)
                                 (:if sym
                                      sym
                                      (mu:intern () :extern base)))
                               ((:lambda (walk) (walk walk (core::read-get-ns)))
                                (:lambda (walk ns)
                                  (:if (core:namespacep ns)
                                       ((:lambda (sym)
                                          (:if (core:null sym)
                                               (walk walk (mu:ns-imp ns))
                                               sym))
                                        (mu:ns-find ns :extern symbol))
                                       ns)))))))
                   (mu:intern ns scope base)))
            ((:lambda (ns)
               (:if (core:null ns)
                  ()
                  (env:find-ns ns)))
             (core::read-symbol-ns symbol))
            (:if (core::read-symbol-externp symbol)
                 :extern
                 :intern)
            (core::read-symbol-name symbol)))))

;;;
;;; fixnums
;;;
(mu::intern core::ns :intern "read-fixnump"
   (:lambda (number)
      ((:lambda (number)
          (core:null
            (core:findl-if
              (:lambda (el)
                 (core:null
                   (core:findl-if
                     (:lambda (digit) (mu:eq el digit))
                     "0123456789")))
              number)))
       (:if (mu:eq #\- (mu:sv-ref number 0))
            (core:substr number 1 (core:1- (mu:sv-len number)))
            number))))

(mu::intern core::ns :intern "read-fixnum"
   (:lambda (number)
      (mu:funcall
        (:lambda (number sign)
           (mu:fixnum* sign
             ((:lambda (loop) (loop loop (mu:sv-len number) 0))
              (:lambda (loop len value)
                 (:if (core:zerop len)
                      value
                      (loop
                       loop
                       (core:1- len)
                       (mu:fixnum+
                        (mu:fixnum* 10 value)
                        (core:positionl (mu:sv-ref number (mu:fixnum- (mu:sv-len number) len)) "0123456789"))))))))
        (:if (mu:eq #\- (mu:sv-ref number 0))
             (mu:cons (core:substr number 1 (core:1- (mu:sv-len number))) (mu:cons -1 ()))
             (mu:cons number (mu:cons 1 ()))))))

;;;
;;; atoms
;;;
(mu::intern core::ns :intern "read-atom"
   (:lambda (stream)
      ((:lambda (loop)
          (loop loop (mu:rd-char stream)))
       (:lambda (loop ch)
          (mu:wr-char ch core::reader-stream)
          ((:lambda (ch)
              ((:lambda (syntax-type)
                  (:if (mu:eq syntax-type 'constituent)
                       (loop loop ch)
                       (:if (mu:eofp stream)
                            ()
                            ((:lambda ()
                                (mu:un-char ch stream)
                                ((:lambda (atom)
                                    (:if (core::read-fixnump atom)
                                         (core::read-fixnum atom)
                                         (core::read-resolve-symbol atom)))
                                 (mu:get-str core::reader-stream)))))))
               (core::read-char-syntax ch)))
           (mu:rd-char stream))))))

;;;
;;; lists
;;;
(mu::intern core::ns :intern "read-list-eol" (mu:symbol "eol"))
(mu::intern core::ns :intern "read-list"
   (:lambda (ch stream)
      ((:lambda (loop)
          (:if (mu:eq ch #\))
               core::read-list-eol
               (loop loop ())))
       (:lambda (loop list)
          ((:lambda (el)
              (:if (mu:eq el core::read-list-eol)
                   list
                   (:if (core:symbolp el)
                      (:if (mu:eq (mu:sy-name el) ".")
                         (loop loop (mu:cons (mu:car list) (core::read stream)))
                         (loop loop (core::append list (mu:cons el ()))))
                      (loop loop (core::append list (mu:cons el ()))))))
           (core::read stream))))))

;;;
;;; strings
;;;
(mu::intern core::ns :intern "read-string"
   (:lambda (ch stream)
     ((:lambda (loop) (loop loop (mu:rd-char stream)))
      (:lambda (loop ch)
        (:if (mu:eq ch #\")
             (mu:get-str core::reader-stream)
             ((:lambda ()
                (mu:wr-char ch core::reader-stream)
                (loop loop (mu:rd-char stream)))))))))

;;;
;;; quote
;;;
(mu::intern core::ns :intern "read-quote"
   (:lambda (ch stream)
     (mu:cons :quote (mu:cons (core::read stream) ()))))

;;;
;;; line comment
;;;
(mu::intern core::ns :intern "read-comment"
   (:lambda (ch stream)
     ((:lambda (loop) (loop loop (mu:rd-char stream)))
      (:lambda (loop ch)
        (:if (mu:eq ch (mu:code-ch #xa))
             ()
             (loop loop (mu:rd-char stream)))))))

;;;
;;; read macros
;;;
(mu::intern core::ns :intern "read-macro"
   (:lambda (stream)
      ((:lambda (ch macro-table)
          (mu:funcall
            (mu:sy-val (core:assoc ch macro-table))
            (mu:cons ch (mu:cons stream ()))))
       (mu:rd-char stream)
       '((#\" . core::read-string)
         (#\# . core::read-sharp)
         (#\' . core::read-quote)
         (#\( . core::read-list)
         (#\) . core::read-list)
         (#\; . core::read-comment)
         (#\, . core::quasiquote)
         (#\` . core::quasiquote)))))

;;;
;;; parser
;;;
(mu::intern core::ns :intern "read-dispatch"
   (:lambda (ch stream)
     (mu:un-char ch stream)
      ((:lambda (dispatch-table)
          (mu:funcall
           (mu:sy-val (core:assoc (core::read-char-syntax ch) dispatch-table))
           (mu:cons stream ())))
       '((constituent . core::read-atom)
         (macro       . core::read-macro)
         (tmacro      . core::read-macro)
         (escape      . core::read-atom)
         (mescape     . core::read-atom)))))

(mu::intern core::ns :intern "read-consume-ws"
   (:lambda (stream)
     ((:lambda (loop) (loop loop (mu:rd-char stream)))
      (:lambda (loop ch)
         (:if (core:null ch)
              ()
              (:if (core:assoc ch core::read-whitespace)
                   (loop loop (mu:rd-char stream))
                   (:if (mu:eq ch #\#)
                        ((:lambda (ch)
                            (:if (mu:eq ch #\|)
                                 ((:lambda ()
                                     (mu:un-char ch stream)
                                     (core::read-sharp #\# stream)
                                     (loop loop (mu:rd-char stream))))
                                 ((:lambda ()
                                     (mu:un-char ch stream)
                                     #\#))))
                         (mu:rd-char stream))
                        (:if (mu:eq ch #\;)
                             ((:lambda ()
                                 (core::read-comment ch stream)
                                 (loop loop (mu:rd-char stream))))
                             ch))))))))

(mu::intern core::ns :intern "read"
   (:lambda (stream)
     (core:errorp-when mu:eofp stream "core:read unexpected end of file")
     ((:lambda (ch)
        (:if (core:null ch)
             ()
             (core::read-dispatch ch stream)))
       (core::read-consume-ws stream))))

