;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; preface repl
;;;
(defun repl (ns)
  (errorp-unless namespacep ns "preface:repl not a namespace")
  (flet ((repl (repl)
          (with-ex
              (lambda (ex)  ;; catch exception and break
                (print-except ex "preface:repl")
                (break ex))
              (lambda ()    ;; main
                (flet ((loop (loop form)
                        (if (eofp t)
                            (system:exit 0)
                            (if (keywordp form)
                                (cond
                                  ((eq form :h)
                                   (format t ";;; help: libmu ~A~%:l - load file~%:m - expand macro~%" mu:version))
                                  ((eq form :m) (format t "~A~%" (macroexpand-1 (read t t ()))))
                                  ((eq form :l)
                                   ((lambda (path)
                                      (if (stringp path)
                                          (load path t ())
                                          (format t ":l ~A is not a file path~%" path)))
                                    (read t t ())))
                                  (t ((lambda ()
                                        (core::break form ())
                                        (write form () ())
                                        (terpri ())))))
                                (format t "~A~%" (eval (compile form)))))
                        (format t "~A> " (ns-name ns))
                        (loop loop (read t t ()))))
                  (format t "~A> " (ns-name ns))
                  (loop loop (read t t ())))))))
    (format t ";;;~%")
    (format t ";;; Eko Lisp ~A~%" preface::eko-version)
    (format t ";;; (preface:repl) :h for help~%")
    (repl repl)))
