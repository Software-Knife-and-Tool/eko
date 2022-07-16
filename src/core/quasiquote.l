;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; quasiquote
;;;

#|
@s Summary
<p></p>
The quasiquote list construction DSL.

@s Quasi Quote
<p></p>
` - quasiquote form
<p></p>
, - eval form
<p></p>
,@ - splice eval'd form

|#

(mu::intern core::ns :intern "quasi-list"
   (:lambda (form)
      ((:lambda (loop) (loop loop form))
       (:lambda (loop list)
           ((:lambda (car)
               (:if (core:null list)
                    ()
                    (:if (mu:eq car core::quasi-comma)
                         ((:lambda (comma-form)
                             (mu:cons 'mu:cons
                               (mu:cons (core:eval comma-form)
                                 (mu:cons (loop loop (mu:nthcdr 2 list))
                                   ()))))
                               (mu:nth 1 list))
                         (:if (mu:eq car core::quasi-comma-at)
                              ((:lambda (comma-at-form)
                                  ;;; (core:warn comma-at-form "comma-at-form")
                                  (core::append
                                    (mu:cons :quote
                                      (mu:cons (core:eval comma-at-form)
                                        ()))
                                    (loop loop (mu:nthcdr 2 list))))
                                  (mu:nth 1 list))
                              (mu:cons 'mu:cons
                                (mu:cons car
                                  (mu:cons (loop loop (mu:cdr list))
                                    ())))))))
            (mu:car list))))))

(mu::intern core::ns :intern "quasi-comma-at" (mu:symbol ",@"))
(mu::intern core::ns :intern "quasi-comma" (mu:symbol ","))
(mu::intern core::ns :intern "quasiquote"
   (:lambda (ch stream)
      (:if (mu:eq ch #\`)
           ((:lambda (form)
               (:if (core:listp form)
                    (core:warn (core::quasi-list form) "quasi")
                    (mu:cons :quote (mu:cons form ()))))
            (core:read stream :t ()))
           ((:lambda (at)
               (:if (mu:eq at #\@)
                    core::quasi-comma-at
                    ((:lambda ()
                        (mu:un-char at stream)
                        core::quasi-comma))))
            (mu:rd-char stream)))))

