;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; strings
;;;
(mu::intern core::ns :extern "schar"
  (:lambda (str index)
     (core:errorp-unless core:stringp str "core:schar: not a string")
     (core:errorp-unless core:fixnump index "core:schar not a string")
    (mu:sv-ref str index)))

(mu::intern core::ns :extern "string="
   (:lambda (x y)
      (core:errorp-unless core:stringp x "core:string= not a string")
      (core:errorp-unless core:stringp y "core:string= not a string")
      (:if (mu:eq x y)
         :t
         (:if (mu:eq (mu:sv-len x) (mu:sv-len y))
            ((:lambda (result)
                (core:charp result))
             (core:foldl
               (:lambda (el acc)
                  (:if (core:null acc)
                       ()
                       (:if (mu:eq el (mu:sv-ref y acc))
                            (core:1+ acc)
                            ())))
                  0
                  x))
            ()))))

;;;
;;; string construction
;;;
(mu::intern core::ns :extern "string"
   (:lambda (x)
      (:if (core:stringp x)
         x
         (:if (core:symbolp x)
            (mu:sy-name x)
            (:if (core:charp x)
               (mu:list-sv :char (mu:cons x ()))
               (core:error x "core::string not a char"))))))

(mu::intern core::ns :extern "string-append"
   (:lambda (str-1 str-2)
      (core:errorp-unless core:stringp str-1 "is not a string (core::string)")
      (core:errorp-unless core:stringp str-2 "is not a string (core::string)")
      (mu:list-sv
        :char
        (core::append (core:sv-list str-1) (core:sv-list str-2)))))

;;; can use slice here?
(mu::intern core::ns :extern "substr"
  (:lambda (x start end)
     (core:errorp-unless core:stringp x "is not a string (core::substr)")
     (core:errorp-unless core:fixnump start "is not a fixnum (core::substr)")
     (core:errorp-unless core:fixnump end "is not a fixnum (core::substr)")
     ((:lambda (list)
         (mu:list-sv :char
           (core:dropl (core:dropr list (mu:fixnum- (mu:length list) (core:1+ end))) start)))
         (core:sv-list x))))

