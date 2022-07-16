---
title: print library
---

@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Reader

@s Reader
<hr><br/>
`core::read` reads forms from the given stream.

<p></p>
<br/>

***core::read*** *stream* => *object*

<p></p>
<br/>

`arguments:`
>
> `stream:` input stream designator
>

`returns:` *object*

<p></p>
<br/>

`Exceptions:`
>
> `type:` if `stream` is not an input stream designator or a reader error
>

--- core-reader --- noWeave

;;;
;;; readtable
;;;
(:const core::read-table
   '((#\0 . constituent)  (#\1 . constituent)  (#\2 . constituent)  (#\3 . constituent)
     (#\4 . constituent)  (#\5 . constituent)  (#\6 . constituent)  (#\7 . constituent)
     (#\8 . constituent)  (#\9 . constituent)  (#\: . constituent)  (#\< . constituent)
     (#\> . constituent)  (#\= . constituent)  (#\? . constituent)  (#\! . constituent)
     (#\@ . constituent)  (#xa . wspace)       (#xd . wspace)       (#xc . wspace)
     (#x20 . wspace)      (#\; . tmacro)       (#\" . tmacro)       (#\# . macro)
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
     (#\z . constituent)))

(:const core::read-char-syntax
   (:lambda (ch)
      (core:assoc ch core::read-table)))

;;;
;;; sharpsign macros
;;;
(:const core::read-sharp-char
   (:lambda (ch stream)
      (core:assert core:streamp stream "core::read-sharp-char requires a stream")
      (mu:rd-byte stream)))

;;; way too busy
(:const core::read-sharp-comment
   (:lambda (ch stream)
      (core:assert core:streamp stream "core::read-sharp-comment requires a stream")
      ((:lambda (loop) (loop loop (mu:rd-byte stream)))
       (:lambda (loop ch)
          (:if (mu:eq ch #\|)
               ((:lambda (ch)
                   (:if (mu:eq ch #\#)
                        ()
                        (loop loop (mu:rd-byte stream))))
                (mu:rd-byte stream))
               (loop loop (mu:rd-byte stream)))))))
      
(:const core::read-sharp-vector
   (:lambda (ch stream)
      (core:assert core:streamp stream "core::read-sharp-vector requires a stream")
      ((:lambda (type)
          (core:assert mu:keyp type "core::read-sharp-vector requires type keyword")
          (mu:list-sv type (core::read-list #\( stream)))
       (core::read-atom stream))))
      
(:const core::read-sharp-table
   '((#\| . core::read-sharp-comment)
     (#\\ . core::read-sharp-char)
     (#\( . core::read-sharp-vector)
     (#\: . core::read-sharp-symbol)))

(:const core::read-sharp
   (:lambda (ch stream)
      (core:assert core:streamp stream "core::read-sharp requires a stream")
      ((:lambda (ch)
          (mu:funcall
            (mu:sy-val (core:assoc ch core::read-sharp-table))
            (mu:cons ch (mu:cons stream ()))))
       (mu:rd-byte stream))))

;;;
;;; symbols
;;;
(:const core::read-namespaces (mu:cons "ix" (mu:cons "core" (mu:cons "mu" (mu:cons "platform" ())))))

(:const core::read-symbol-keywordp
   (:lambda (name)
      (core:assert core:stringp name "core::read-symbol-keywordp requires a string")
      (mu:eq #\: (mu:sv-ref name 0))))

(:const core::read-symbol-externp
   (:lambda (name)
      (core:assert core:stringp name "core::read-symbol-externp requires a string")
      (mu:eq (core:string-findl name #\:) (core:string-findr name #\:))))
      
(:const core::read-symbol-ns
   (:lambda (name)
      (core:assert core:stringp name "core::read-symbol-ns requires a string")
      ((:lambda (colon)
          (:if (core:null colon)
               ""
               (core:substr name 0 (mu:fixnum- colon 1))))
       (core:string-findl name #\:))))

(:const core::reader-stream (mu:st-ostr ""))
(:const core::read-symbol-name
   (:lambda (name)
      (core:assert core:stringp name "core::read-symbol-name requires a string")
      ((:lambda (colon)
          (:if (core:null colon)
               name
               (core:substr
                 name
                 (mu:fixnum+ colon 1)
                 (mu:fixnum- (mu:sv-len name) 1))))
       (core:string-findr name #\:))))

(:const core::read-resolve-symbol
   (:lambda (symbol-str)
      (core:assert core:stringp symbol-str "core::read-resolve-symbol requires a string")
      (:if (core::read-symbol-keywordp symbol-str)
           (mu:key (core::read-symbol-name symbol-str))
           ((:lambda (ns base scope)
               (:if (core:namespacep ns)
                  (:if (mu:eq ns (mu:find-ns ""))
                       ((:lambda (sym) sym)
                        (:if (core:string= base "t")
                           :t
                           (:if (core:string= base "nil")
                              :nil
                              ((:lambda (sym)
                                  (:if sym sym (mu:intern (mu:find-ns "") :extern base)))
                               (core:foldl
                                (:lambda (ns acc)
                                   (:if acc acc (mu:ns-find (mu:find-ns ns) :extern symbol-str)))
                                 ()
                                 core::read-namespaces)))))
                       (mu:intern ns scope base))
                  (core:error symbol-str "core:resolve-symbol requires a namespace")))
            (mu:find-ns (core::read-symbol-ns symbol-str))
            (core::read-symbol-name symbol-str)
            (:if (core::read-symbol-externp symbol-str) :extern :intern)))))

;;;
;;; fixnums
;;;
(:const core::read-fixnump
   (:lambda (number)
      (core:assert core:stringp number "core::read-fixnump requires a string")
      ((:lambda (number)
          (core:null
            (core:sv-findl-if
              (:lambda (el)
                 (core:null
                   (core:sv-findl-if
                     (:lambda (digit) (mu:eq el digit))
                     "0123456789")))
              number)))
       (:if (mu:eq #\- (mu:sv-ref number 0))
            (core:substr number 1 (mu:fixnum- (mu:sv-len number) 1))
            number))))

(:const core::read-fixnum
   (:lambda (number)
      (core:assert core:stringp number "core::read-fixnum requires a string")
      (mu:funcall
        (:lambda (number sign)
           (mu:fixnum* sign
           ((:lambda (loop) (loop loop (mu:sv-len number) 0))
               (:lambda (loop len value)
                  (:if (core:zerop len)
                       value
                       (loop
                        loop
                        (mu:fixnum- len 1)
                        (mu:fixnum+
                          (mu:fixnum* 10 value)
                          (mu:fixnum- (mu:sv-ref number (mu:fixnum- (mu:sv-len number) len)) #\0))))))))
        (:if (mu:eq #\- (mu:sv-ref number 0))
            (mu:cons (core:substr number 1 (mu:fixnum- (mu:sv-len number) 1)) (mu:cons -1 ()))
            (mu:cons number (mu:cons 1 ()))))))

;;;
;;; atoms
;;;
(:const core::read-atom
   (:lambda (stream)
      (core:assert core:streamp stream "core::read-atom requires a stream")
      ((:lambda (loop) (loop loop (mu:rd-byte stream)))
       (:lambda (loop ch)
          (mu:wr-byte ch core::reader-stream)
          ((:lambda (ch)
              ((:lambda (syntax-type)
                  (:if (mu:eq syntax-type 'constituent)
                       (loop loop ch)
                       ((:lambda ()
                           (:if (mu:eofp stream)
                              ()
                              ((:lambda ()
                                  (core:assert core:fixnump ch "core::read-atom requires a char")
                                  (mu:un-byte ch stream)
                                  ((:lambda (atom)
                                     (:if (core::read-fixnump atom)
                                        (core::read-fixnum atom)
                                        (core::read-resolve-symbol atom)))
                                   (mu:get-str core::reader-stream)))))))))
               (core::read-char-syntax ch)))
           (mu:rd-byte stream))))))

;;;
;;; lists
;;;
(:const core::read-list-eol (mu:symbol "eol"))
(:const core::read-list
   (:lambda (ch stream)
      (core:assert core:fixnump ch "core::read-list requires a character")
      (core:assert core:streamp stream "core::read-list requires a stream")
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
                         (loop loop (mu:cons (mu:car list) (core::read stream :t)))
                         (loop loop (core::append list (mu:cons el ()))))
                      (loop loop (core::append list (mu:cons el ()))))))
           (core::read stream :t))))))

;;;
;;; strings
;;;
(:const core::read-string
   (:lambda (ch stream)
      (core:assert core:fixnump ch "core::read-string requires a character")
      (core:assert core:streamp stream "core::read-string requires a stream")
      ((:lambda (loop) (loop loop (mu:rd-byte stream)))
       (:lambda (loop ch)
          (:if (mu:eq ch #\")
               (mu:get-str core::reader-stream)
               ((:lambda ()
                   (mu:wr-byte ch core::reader-stream)
                   (loop loop (mu:rd-byte stream)))))))))

;;;
;;; quote
;;;
(:const core::read-quote
   (:lambda (ch stream)
      (core:assert core:streamp stream "core::read-quote requires a stream")
      (mu:cons :quote (mu:cons (core::read stream :t) ()))))

;;;
;;; line comment
;;;
(:const core::read-comment
   (:lambda (ch stream)
      (core:assert core:streamp stream "core::read-comment requires a stream")
      ((:lambda (loop) (loop loop (mu:rd-byte stream)))
       (:lambda (loop ch)
          (:if (mu:eq ch #xa)
               ()
               (loop loop (mu:rd-byte stream)))))))

;;;
;;; read macros
;;;
(:const core::read-macro-table
   '((#\" . core::read-string)
     (#\# . core::read-sharp)
     (#\' . core::read-quote)
     (#\( . core::read-list)
     (#\) . core::read-list)
     (#\; . core::read-comment)
     (#\, . core::quasiquote)
     (#\` . core::quasiquote)))

(:const core::read-macro
   (:lambda (stream)
      (core:assert core:streamp stream "core::read-tmacro requires a stream")
      ((:lambda (ch)
          (mu:funcall
            (mu:sy-val (core:assoc ch core::read-macro-table))
            (mu:cons ch (mu:cons stream ()))))
       (mu:rd-byte stream))))

;;;
;;; parser
;;;
(:const core::read-dispatch-table
   '((constituent . core::read-atom)
     (macro       . core::read-macro)
     (tmacro      . core::read-macro)
     (escape      . core::read-atom)
     (mescape     . core::read-atom)))

(:const core::read-dispatch
   (:lambda (ch stream)
      (core:assert core:fixnump ch "core::read-dispatch requires a char")
      (core:assert core:streamp stream "core::read-dispatch requires a stream")
      (mu:un-byte ch stream)
      (mu:funcall
        (mu:sy-val (core:assoc (core::read-char-syntax ch) core::read-dispatch-table))
        (mu:cons stream ()))))

(:const core::read-consume-ws
  (:lambda (stream)
     (core:assert core:streamp stream "core::read-consume-ws requires a stream")
     ((:lambda (loop) (loop loop (mu:rd-byte stream)))
      (:lambda (loop ch)
         (:if (core:null ch)
            ()
            (:if (core:sv-findl-if (:lambda (ws) (mu:eq ws ch)) #(:byte 32 10))
                 (loop loop (mu:rd-byte stream))
                 ch))))))

(:const core::read
   (:lambda (stream eof-error)
      (core:assert core:streamp stream "core::read requires a stream")
      ((:lambda (ch)
          (:if (core:null ch)
             (:if eof-error
                  (core:error stream "core::read early end of file")
                  ())
             (core::read-dispatch ch stream)))
       (core::read-consume-ws stream))))

(:const core:read
  (:lambda (stream eof-error)
     ((:lambda (stream)
         ((:lambda (form)
             (:if (mu:eq form core::read-list-eol)
                (core:error :nil "core:read unmatched close parenthesis")
                form))
          (core::read stream eof-error)))
      (core::stream-designator stream))))
---

--- core reader --- noWeave
@{core-reader}
---

--- core:read.l --- noWeave
@{core reader}
---