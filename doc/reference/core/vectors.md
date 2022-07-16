---
title: print library
---

@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Vectors

@s Vectors
<p></p>
<br/>
Core vectors are simple specialized vectors for the types
`:t`, `:byte`, `:fixnum`, `:float`, and `:double`.

@s Vector Reduction
<p></p>
Vector folds and find.

--- vector reduction
(:const core:sv-foldl
   (:lambda (fn init vec)
      (core:assert core:functionp fn "is not a function (core::sv-foldl)")
      (core:assert core:vectorp vec "is not a list (core:sv-foldl)")
      ((:lambda (fold) (fold fold init 0))
       (:lambda (fold acc index)
          (:if (mu:fixnum< index (mu:sv-len vec))
             (fold fold (fn (mu:sv-ref vec index) acc) (mu:fixnum+ index 1))
             acc)))))

(:const core:sv-foldr
   (:lambda (fn init vec)
      (core:assert core:functionp fn "is not a function (core::sv-foldr)")
      (core:assert core:vectorp vec "is not a list (core:sv-foldr)")
      ((:lambda (fold) (fold fold init (mu:fixnum- (mu:sv-len vec) 1)))
       (:lambda (fold acc index)
          (:if (mu:fixnum< -1 index)
             (fold fold (fn (mu:sv-ref vec index) acc) (mu:fixnum- index 1))
             acc)))))

(:const core:sv-findl-if
   (:lambda (predicate vec)
      (core:assert core:functionp predicate "core::sv-findl-if requires a function")
      (core:assert core:vectorp vec "core:sv-findl-if requires a vector")
      (core:sv-foldl
        (:lambda (el acc) (:if (predicate el) el acc))
        ()
        vec)))

(:const core:sv-findr-if
   (:lambda (predicate vec)
      (core:assert core:functionp predicate "core::sv-findr-if requires a function")
      (core:assert core:vectorp vec "core:sv-findr-if requires a vector")
      (core:sv-foldr
        (:lambda (el acc) (:if (predicate el) el acc))
        ()
        vec)))
---

@s Strings
<p></p>
This is the first step to UTF-8 and UTF-16 strings. We'll need *defstruct* to get a real
independent type, but we'll supply some string operations that hide the underlying
byte string impolementation. We also need a *char* type distinct from byte/fixnum.
<br/><br/>

@s String Predicates

--- string predicates
(:const core:stringp
   (:lambda (v)
      (:if (core:vectorp v)
         (mu:eq :byte (mu:sv-type v))
         ())))

(:const core:string=
   (:lambda (x y)
      (core:assert core:stringp x "core:string= requires a string")
      (core:assert core:stringp y "core:string= requires a string")
      (:if (mu:eq x y)
         :t
         (:if (mu:eq (mu:sv-len x) (mu:sv-len y))
            ((:lambda (result)
                (core:fixnump result))
             (core:sv-foldl
               (:lambda (el acc)
                  (:if (core:null acc)
                       ()
                       (:if (mu:eq el (mu:sv-ref y acc))
                            (mu:fixnum+ acc 1)
                            ())))
                  0
                  x))
            ()))))
---

@s String Construction

--- string construction
(:const core:string
   (:lambda (x)
      (:if (core:stringp x)
         x
         (:if (core:symbolp x)
            (mu:sy-name x)
            (:if (core:fixnump x)
               (mu:list-sv :byte (mu:cons x ()))
               (core:error x "core::string requires a fixnum"))))))

(:const core:string-append
   (:lambda (str-1 str-2)
      (core:assert core:stringp str-1 "is not a string (core::string)")
      (core:assert core:stringp str-2 "is not a string (core::string)")
      (mu:list-sv
        :byte
        (core::append (core:sv-list str-1) (core:sv-list str-2)))))

(:const core:substr
  (:lambda (x start end)
     (core:assert core:stringp x "is not a string (core::substr)")
     (core:assert core:fixnump start "is not a fixnum (core::substr)")
     (core:assert core:fixnump end "is not a fixnum (core::substr)")
     ((:lambda (list)
         (mu:list-sv :byte
           (core:dropl (core:dropr list (mu:fixnum- (mu:length list) (mu:fixnum+ end 1))) start)))
         (core:sv-list x))))
---

@s String Reduction

--- string reduction
(:const core:schar
  (:lambda (x index)
    (core:assert core:stringp x "is not a string (core::schar)")
    (mu:sv-ref x index)))

(:const core:string-length
  (:lambda (x)
    (core:assert core:stringp x "is not a string (core::string-length)")
    (mu:sv-len x)))

(:const core:string-findl
   (:lambda (str ch)
      (core:assert core:stringp str "core:string-findl requires a string")
      (core:assert core:fixnump ch "core:string-findl requires a character")
      ((:lambda (pos)
          (:if (core:consp pos)
               (mu:car pos)
               ()))
       (core:sv-foldl
         (:lambda (el acc)
            (:if (core:consp acc)
                 acc
                 (:if (mu:eq el ch)
                      (mu:cons acc ())
                      (mu:fixnum+ 1 acc))))
         0
         str))))

(:const core:string-findr
   (:lambda (str ch)
      (core:assert core:stringp str "core:string-findr requires a string")
      (core:assert core:fixnump ch "core:string-findr requires a character")
      ((:lambda (pos)
          (:if (core:consp pos)
               (mu:car pos)
               ()))
       (core:sv-foldr
         (:lambda (el acc)
            (:if (core:consp acc)
                 acc
                 (:if (mu:eq el ch)
                      (mu:cons acc ())
                      (mu:fixnum- acc 1))))
         (mu:fixnum- (mu:sv-len str) 1)
         str))))
---

@s String Utilities

--- string utilities
(:const core:print-str
  (:lambda (bstr stream escape)
    (:if escape
      (mu:wr-byte #\" stream)
      ())
    (core:sv-foldl
      (:lambda (ch acc)
         (mu:wr-byte ch stream))
      ()
      bstr)
    (:if escape
      (mu:wr-byte #\" stream)
      ())
      bstr))
---

--- core-vectors --- noWeave
@{vector reduction}
@{string predicates}
@{string construction}
@{string reduction}
@{string utilities}
---

--- core:vectors.l --- noWeave
@{core-vectors}
---
