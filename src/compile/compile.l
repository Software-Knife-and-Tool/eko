;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;;  compile: eko compiler framework
;;;
;;;  compile namespace
;;;
(preface:require-lib common)
(core:in-namespace (mu:ns "compile" (env:find-ns "common")))

(defun compile::compile-form (form)
  (flet ((parse (form)
                (if (consp form)
                    (mapcar compile::compile-form form)
                    form))
         (const (form)
                (cond
                  ((and (symbolp form)
                        (boundp form)
                        (not (symbolp (symbol-value form)))
                        (not (consp (symbol-value form))))
                   (preface:format t ";;; constant propagation: ~A ~A~%" form (symbol-value form))
                   (symbol-value form))
                  (t form)))
         (review (form)
                 form))
        (foldl
         (lambda (fn intermediate)
           (funcall fn intermediate))
         form
         (list parse const))))

#| core: letf function proxy wrong arity #165
(defun compile-file (input-path output-path &rest options)
  (errorp-unless stringp input-path "compile-file: not a pathname")
  (errorp-unless stringp output-path "compile-file: not a pathname")
  (common:with-open-file src input-path :input
    (preface:format t "loop: ~A~%" "loop")
    (letf ((loop (form)
             (preface:format t "loop: ~A~%" loop)
             (cond
               ((eofp src))
               (t (compile::compile-form form)
                  (funcall loop (read src () ()))))))
          (preface:format t "loop: ~A~%" loop)
          (loop (read src () ())))))
|#

;;;
;;; compile-file
;;;
(defun compile-file (input-path output-path &rest options)
  (errorp-unless stringp input-path "compile-file: not a pathname")
  (errorp-unless stringp output-path "compile-file: not a pathname")
  (common:with-open-file src input-path :input
    ((lambda (walk) (walk walk (read src () ())))
     (lambda (walk form)
       (cond
         ((eofp src))
(t (compile::compile-form form) (walk walk (read src () ()))))))))

