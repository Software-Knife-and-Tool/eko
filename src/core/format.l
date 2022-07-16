;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; format
;;;
(mu::intern core::ns :extern "format"
   (:lambda (stream fmt-string args)
      (core:errorp-unless core:stringp fmt-string "core:format not a string")
      (core:errorp-unless core:listp args "core:format not a list")
      ((:lambda (dest fmt)
         ((:lambda (loop)
            ((:lambda (value)
               (:if (core:stringp value)
                    value
                    ()))
            (loop loop args)))
           (:lambda (loop args)
              ((:lambda (ch)
                  (:if (core:null ch)
                     (:if stream
                          dest
                          (mu:get-str dest))
                     (:if (mu:eq ch #\~)
                          ((:lambda (dir)
                              (core:errorp-unless core:charp dir "core:format eof while processing directive")
                              (:if (mu:eq dir #\~)
                                   ((:lambda () (mu:wr-char #\~ dest) (loop loop args)))
                                   (:if (mu:eq dir #\%)
                                        ((:lambda () (mu:wr-byte #xa dest) (loop loop args)))
                                        (:if (mu:eq dir #\A)
                                             (mu:write (mu:car args) dest ())
                                             (:if (mu:eq dir #\C)
                                                  (mu:write (mu:car args) dest ())
                                                  (:if (mu:eq dir #\S)
                                                       (mu:write (mu:car args) dest :t)
                                                       (:if (mu:eq dir #\W)
                                                            (mu:write (car args) dest :t)
                                                            (core:error dir "core:format unrecognized format directive")))))))
                              (loop loop (mu:cdr args)))
                           (mu:rd-char fmt))
                          ((:lambda ()
                              (mu:wr-char ch dest)
                              (loop loop args))))))
               (mu:rd-char fmt)))))
       (:if (core:null stream)
            (mu:open :string :output "")
            (:if (mu:eq stream :t)
                 mu:std-out
                 (:if (core:streamp stream)
                      stream
                      (core:error stream "core:format not a stream designator"))))
       (mu:open :string :input fmt-string))))

