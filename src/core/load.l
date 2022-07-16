;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; loader
;;;
(mu::intern core::ns :extern "load"
   (:lambda (source verbose print)
      (core:errorp-unless core:stringp source "core:load not a file designator")
      ((:lambda (ifs)
          (core:errorp-unless core:streamp ifs "core:load cannot open input file")
          ((:lambda (load-loop)
              (:if verbose
                 ((:lambda ()
                     (mu:write (core:string-append ";;; loading " source) mu:std-out ()) 
                     (mu:wr-byte #xa mu:std-out)))
                 ())
              (mu:with-ex
                (:lambda (ex)
                   (mu:write "load exception" mu:err-out ()) (mu:wr-byte #xa mu:err-out)
                   (core:print-except ex "core:load")
                   (mu:write ";;; in file " mu:err-out ())
                   (mu:write source mu:err-out ())
                   (mu:wr-byte #xa mu:err-out))
                (:lambda ()
                  (:if (mu:eofp ifs)
                       :t
                       (load-loop load-loop (core:read ifs :t ())))))
              :t)
           (:lambda (loop form)
             ((:lambda ()
                ((:lambda (val)
                   (:if print
                        ((:lambda ()
                           (mu:write val mu:std-out ())
                           (mu:wr-byte #xa mu:std-out)))
                        ()))
                 (core:eval (core:compile form)))
                (:if (mu:eofp ifs)
                     ()
                     (loop loop (core:read ifs :t ()))))))))
       (mu:open :file :input source))))

