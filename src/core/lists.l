;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; lists
;;;
(mu::intern core::ns :extern "mapc"
   (:lambda (fn list)
      (core:errorp-unless core:functionp fn "core:mapc not a function")
      (core:errorp-unless core:listp list "core:mapc not a list")
      (core:foldl (:lambda (el acc) (fn el) acc) list list)))

(mu::intern core::ns :extern "mapcar"
   (:lambda (fn list)
      (core:errorp-unless core:functionp fn "core:mapcar not a function")
      (core:errorp-unless core:listp list "core:mapcar not a list")
      ((:lambda (walk) (walk walk list))
       (:lambda (walk list)
          (:if list
               (mu:cons (fn (mu:car list)) (walk walk (mu:cdr list)))
               ())))))

(mu::intern core::ns :extern "mapl"
   (:lambda (fn list)
      (core:errorp-unless core:functionp fn "core:mapl not a function")
      (core:errorp-unless core:listp list "core:mapl not a list")
      ((:lambda (walk) (walk walk list))
       (:lambda (walk lst)
          (fn lst)
          (:if lst
               (walk walk (mu:cdr lst))
               list)))))

(mu::intern core::ns :extern "maplist"
   (:lambda (fn list)
      (core:errorp-unless core:functionp fn "core:maplist not a function")
      (core:errorp-unless core:listp list "core:maplist not a list")
      ((:lambda (walk) (walk walk list))
       (:lambda (walk list)
          (:if list
               (mu:cons (fn list) (walk walk (mu:cdr list)))
               ())))))

(mu::intern core::ns :intern "append"
   (:lambda (list-1 list-2)
      ; (core:errorp-unless core:listp list-1 "core:append: not a list")
      ; (core:errorp-unless core:listp list-2 "core:append: not a list")
      (core:foldr
        mu:cons
        list-2
        list-1)))

;;; generalize this to coerce?
(mu::intern core::ns :extern "sv-list"
   (:lambda (vec)
      (core:errorp-unless core:vectorp vec "core:sv-list not a vector")
      (core:foldl
        mu:cons
        ()
        (core:foldl
          mu:cons
          ()
          vec))))

(mu::intern core::ns :extern "dropl"
   (:lambda (list n)
      (core:errorp-unless core:listp list "core:dropl not a list")
      (core:errorp-unless core:fixnump n "core:dropl not a fixnum")
      (mu:nthcdr n list)))

(mu::intern core::ns :extern "dropr"
   (:lambda (list n)
      (core:errorp-unless core:listp list "core:dropr: not a list")
      (core:errorp-unless core:fixnump n "core:dropr: not a fixnum")
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

(mu::intern core::ns :extern "assoc"
  (:lambda (item list)
     (core:errorp-unless core:listp list "core:assoc: not a function")
     (core:foldl
       (:lambda (el acc) (:if (mu:eq item (mu:car el)) (mu:cdr el) acc))
       ()
       list)))

(mu::intern core::ns :extern "last"
   (:lambda (list)
     (core:errorp-unless core:listp list "core:last: not a list")
     (:if list
          (mu:nthcdr (core:1- (mu:length list)) list)
          ())))

(mu::intern core::ns :extern "lastn"
   (:lambda (list nth)
     (core:errorp-unless core:listp list "core:assoc: not a list")
     (core:errorp-unless (:lambda (nth) (mu:fixnum< 0 nth)) nth "core:lastn not a positive nixnum")      
     (:if (core:zerop nth)
          ()
          (mu:nthcdr (mu:fixnum- (mu:length list) nth) list))))

