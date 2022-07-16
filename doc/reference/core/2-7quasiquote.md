---
title: print library
---

@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Quasiquote

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

--- core quasiquote
(:const core::quasi-list
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

(:const core::quasi-comma-at (mu:symbol ",@"))
(:const core::quasi-comma (mu:symbol ","))
(:const core::quasiquote
   (:lambda (ch stream)
      (:if (mu:eq ch #\`)
           ((:lambda (form)
               (:if (core:listp form)
                    (core:warn (core::quasi-list form) "quasi")
                    (mu:cons :quote (mu:cons form ()))))
            (core:read stream :t))
           ((:lambda (at)
               (:if (mu:eq at #\@)
                    core::quasi-comma-at
                    ((:lambda ()
                        (mu:un-byte at stream)
                        core::quasi-comma))))
            (mu:rd-byte stream)))))
---

--- core-quasiquote --- noWeave
@{core quasiquote}
---

--- core:quasiquote.l --- noWeave
@{core-exceptions}
---
