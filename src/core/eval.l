;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; eval: function calls
;;;

;;; need a with-ex here to clean up the frame stack
;;; if we take an exception while calling the closure
;;; function
(mu::intern core::ns :intern "funcall-closure"
   (:lambda (fn args)
      ((:lambda (desc)
          ((:lambda (closure)
              (:if closure
                   ((:lambda ()
                       (core:mapc env:lex-psh closure)
                       ((:lambda (value)
                           (core:mapc env:lex-pop closure)
                           value)
                        (mu:funcall fn args))))
                   (mu:funcall fn args)))
           (core::lambda-closure desc)))
        (core::fn-extension fn))))

(mu::intern core::ns :intern "funcall"
   (:lambda (fn args)
      (core:funcall fn (core:mapcar core:eval args))))
      
(mu::intern core::ns :extern "funcall"
   (:lambda (fn args)
      (core:errorp-unless core:functionp fn "core:funcall not a function")
      (core:errorp-unless core:listp args "core:funcall not a list")
      (:if (core::fn-extension fn)
           (:if (core::fn-restp fn)
                ((:lambda (nreqs nargs)
                   ((:lambda (reqs rest)
                      ((:lambda (args) (core::funcall-closure fn args))
                       (core::append reqs (mu:cons rest ()))))
                    (core:dropr args (mu:fixnum- nargs nreqs))
                    (core:dropl args nreqs)))
                 (core::fn-nreqs fn)
                 (mu:length args))
                (core::funcall-closure fn args))
           (mu:funcall fn args))))

(mu::intern core::ns :extern "eval"
  (:lambda (form)
    (:if (core:consp form)
         ((:lambda (fn args)
            (:if (core:functionp fn)
                 (core:funcall fn (core:mapcar core:eval args))
                 (:if (core:symbolp fn)
                      (:if (mu:eq :quote fn)
                           (mu:car args)
                           (core:funcall (mu:sy-val fn) (core:mapcar core:eval args)))
                      (core:error fn "core:eval: not a function designator"))))
          (mu:car form)
          (mu:cdr form))
         (mu:eval form))))

