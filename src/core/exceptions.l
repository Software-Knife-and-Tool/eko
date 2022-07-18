;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; exceptions and break
;;;
(mu::intern core::ns :extern "print-except"
   (:lambda (ex by)
     (core:write ";;; exception raised by " mu:err-out ())
     (core:write by mu:err-out ())
     (core:write " with " mu:err-out ())
     (core:write (mu::view ex) mu:err-out ())
     (core:terpri mu:err-out)
     (core:write ";;;     in " mu:err-out ())
     (core:write (mu::view ex) mu:err-out ())
     (core:terpri mu:err-out)))

(mu::intern core::ns :intern "break"
   (:lambda (cmd ex)
      (:if (mu:eq cmd :h)
         ((:lambda ()
             (core:write "break help:" () ()) (core:terpri ())
             (core:write "x - exit process" () ()) (core:terpri ())
             (core:write "r - return from break" () ()) (core:terpri ())
             (core:write "s - print stack frames" () ()) (core:terpri ())
             ()))
         cmd)))

(mu::intern core::ns :intern "print-frame"
   (:lambda (frame)
      ((:lambda (fn args)
         (core:format :t
                "(~A ~A)~%"
                (mu:cons (:if (core:symbolp (core::fn-form fn))
                              (core::fn-form fn)
                              (core:string-append :intern "lambda-" (core:symbol-name (core::fn-frame-id fn))))
                  (mu:cons (core:sv-list args) ()))))
       (mu:car frame)
       (mu:cdr frame))))

(mu::intern core::ns :extern "break"
   (:lambda (except)
     (core:errorp-unless core:exceptionp except "core:break not an exception")
     ((:lambda (loop)
        (core:write ";;; entering break loop, :h for commands" () ()) (core:terpri ())
        (core:write "break> " () ())
        (loop loop (core:read :t () ())))
      (:lambda (loop form)
        ((:lambda (form)
           (:if (mu:eq form :x)
                (system:exit 0)
                (:if (mu:eq form :s)
                     ((:lambda ()
                        (core:mapc
                         (:lambda (frame) (core::print-frame frame))
                         (mu:car (env:context)))
                        (core:write "break> " () ())
                        (loop loop (core:read :t () ()))))
                     (:if (mu:eq form :r)
                          ()
                          ((:lambda ()
                             (core:write (core:eval (core:compile form)) () :t) (core:terpri ())
                             (core:write "break> " () ())
                             (loop loop (core:read :t () ()))))))))
         (:if (mu:keysymp form)
              (core::break form except)
              form))))))

