---
title: print library
---

@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Lists

@s Lists

@s List Predicates
<hr>
--- list predicates
(:const core:null (:lambda (t) (mu:eq :nil t)))
(:const core:consp (:lambda (t) (mu:eq :cons (mu:type-of t))))
(:const core:listp (:lambda (t) (:if (core:consp t) :t (core:null t))))
---

@s List Construction
<hr>
<p></p>
The usual collection of maps.

--- list construction
(:const core:mapc
   (:lambda (fn list)
      (core:assert core:functionp fn "core:mapc requires a function")
      (core:assert core:listp list "core:mapc requires a list")
      (core:foldl (:lambda (el acc) (fn el) acc) list list)))

(:const core:mapcar
   (:lambda (fn list)
      (core:assert core:functionp fn "core:mapcar requires a function")
      (core:assert core:listp list "core:mapcar requires a list")
      ((:lambda (walk) (walk walk list))
       (:lambda (walk list)
          (:if list
               (mu:cons (fn (mu:car list)) (walk walk (mu:cdr list)))
               ())))))

(:const core:mapl
   (:lambda (fn list)
      (core:assert core:functionp fn "core:mapl requires a function")
      (core:assert core:listp list "core:mapl requires a list")
      ((:lambda (walk) (walk walk list))
       (:lambda (walk lst)
          (fn lst)
          (:if lst
               (walk walk (mu:cdr lst))
               list)))))

(:const core:maplist
   (:lambda (fn list)
      (core:assert core:functionp fn "core:maplist requires a function")
      (core:assert core:listp list "core:maplist requires a list")
      ((:lambda (walk) (walk walk list))
       (:lambda (walk list)
          (:if list
               (mu:cons (fn list) (walk walk (mu:cdr list)))
               ())))))

(:const core::list
   (:lambda (x y)
      (mu:cons x (mu:cons y ()))))

(:const core::append
   (:lambda (list-1 list-2)
      (core:assert core:listp list-1 "core::append requires a list")
      (core:assert core:listp list-2 "core::append requires a list")
      (core:foldr
        mu:cons
        list-2
        list-1)))

(:const core::zip
   (:lambda (list-1 list-2)
      (core:assert core:listp list-1 "core:zip requires a list")
      (core:assert core:listp list-2 "core:zip requires a list")
      ((:lambda (loop) (loop loop list-1 list-2))
       (:lambda (loop l1 l2)
          (:if (core:null l1)
             ()
             (core::append
               (mu:cons (core::list (mu:car l1) (mu:car l2)) ())
               (loop loop (mu:cdr l1) (mu:cdr l2))))))))

(:const core:copy
   (:lambda (list)
      (core:assert core:listp list "core:copy requires a list")
      ((:lambda (loop) (loop loop list))
       (:lambda (loop list)
          (:if (mu:eq list ())
               ()
               (mu:cons (mu:car list) (loop loop (mu:cdr list))))))))

(:const core:reverse
   (:lambda (list)
      (core:assert core:listp list "core:reverse requires a list")
      ((:lambda (loop) (loop loop list))
       (:lambda (loop list)
          (:if (mu:eq list ())
               ()
               (core::append (loop loop (mu:cdr list)) (mu:cons (mu:car list) ())))))))

(:const core:sv-list
   (:lambda (vec)
      (core:assert core:vectorp vec "core:sv-list requires a vector")
      (core:foldl
        mu:cons
        ()
        (core:sv-foldl
          mu:cons
          ()
          vec))))

(:const core:dropl
   (:lambda (list n)
      (core:assert core:listp list "core:dropl requires a list")
      (core:assert core:fixnump n "core:dropl requires a fixnum")
      (mu:nthcdr n list)))

(:const core:dropr
   (:lambda (list n)
      (core:assert core:listp list "core:dropr requires a list")
      (core:assert core:fixnump n "core:dropr requires a fixnum")
      (core:foldl
        mu:cons
        ()
        ((:lambda (len)
            (core:foldl
              (:lambda (el last)
                 (:if (mu:fixnum< (mu:length last) (mu:fixnum- (mu:length list) n))
                      (mu:cons el last)
                      last))
                 ()
              list))
         (mu:length list)))))
---

@s List Reduction
<hr>
<p></p>
The usual collection of folds.

--- list reduction
(:const core:foldl
   (:lambda (fn init list)
      (core:assert core:functionp fn "core:foldl requires a function")
      (core:assert core:listp list "core:foldl requires a list")
      ((:lambda (fold) (fold fold init list))
       (:lambda (fold acc list)
          (:if (core:null list)
             acc
             (fold fold (fn (mu:car list) acc) (mu:cdr list)))))))

(:const core:foldr
   (:lambda (fn init list)
      (core:assert core:functionp fn "core:foldr requires a function")
      (core:assert core:listp list "core:foldr requires a list")
      ((:lambda (fold) (fold fold list))
       (:lambda (fold list)
          (:if (core:null list)
               init
               (fn (mu:car list) (fold fold (mu:cdr list))))))))

(:const core:find-if
   (:lambda (predicate list)
      (core:assert core:functionp predicate "core:find-if requires a function")
      (core:assert core:listp list "core:find-if requires a function")
      (core:foldl
        (:lambda (el acc) (:if (predicate el) el acc))
        ()
        list)))

(:const core:filter
  (:lambda (predicate list)
     (core:assert core:functionp predicate "core:filter requires a function")
     (core:assert core:listp list "core:filter requires a list")
     ((:lambda (loop) (loop loop list))
      (:lambda (loop list)
         (:if (core:null list)
              ()
              (:if (predicate (mu:car list))
                   (mu:cons (mu:car list) (loop loop (mu:cdr list)))
                   (loop loop (mu:cdr list))))))))

(:const core:assoc
  (:lambda (item list)
     (core:assert core:listp list "core:assoc requires a function")
     (core:foldl
       (:lambda (el acc) (:if (mu:eq item (mu:car el)) (mu:cdr el) acc))
       ()
       list)))
---

--- core-lists --- noWeave
@{list predicates}
@{list construction}
@{list reduction}
---

--- core:lists.l --- noWeave
@{core-lists}
---
