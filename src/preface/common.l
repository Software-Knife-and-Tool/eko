;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; dotimes macro
;;;
(defmacro dotimes (ctr-list &rest body)
  (errorp-unless consp ctr-list "dotimes not an init list")
  (let ((init (nth 1 ctr-list))
        (ctr (car ctr-list))
        (loop-gsym (gensym)))
    (errorp-unless symbolp (car ctr-list) "dotimes not a symbol")  
    (list
      (list 'lambda (list loop-gsym) (list loop-gsym loop-gsym (list 'eval init)))
      (list 'lambda
            (list loop-gsym ctr)
            (list 'if list zerop ctr) ()
            (list 'progn
                  (list* 'progn body)
                  (list loop-gsym loop-gsym
                        (list 'core:1- ctr)))))))

;;;
;;; dolist macro
;;;
(defmacro dolist (init-list &rest body)
  (errorp-unless consp init-list "dolist not an init list")
  (let ((init (nth 1 init-list))
        (sym (car init-list)))
    (errorp-unless symbolp sym "dolist not a symbol")  
    (errorp-unless listp init "dolist not a list")
    (list
      (list 'lambda ()
        (list
         'mapc
         (list* 'lambda (list sym) body)
         (list 'eval init))
        ()))))

;;;
;;; with-open-file macro
;;;
(defmacro with-open-file (symbol path dir &rest body)
  (list 'let (list (list symbol (list core:open dir path)))
    (list 'with-ex
      (list 'lambda (list 'ex)
        (list 'print-except 'ex "with-open-file")
        (list 'close symbol))
      (list* 'lambda () body))))
