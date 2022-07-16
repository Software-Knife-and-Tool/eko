;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; common printing
;;;
(defun prin1 (form stream)
   (write form stream t))

(defun princ (form stream)
   (write form stream ()))

;;;
;;; common pprint
;;;
(defun common::spaces (n stream)
  (errorp-unless fixnump n "")
  (errorp-unless streamp stream "")
  ((lambda (loop) (loop loop n))
   (lambda (loop n)
     (if (zerop n)
         ()
         ((lambda ()
            (write " " stream ())
            (loop loop (1- n))))))))

(defun pprint (form stream)
  (let ((pprint-threshold 3)
        (pprint-indents
         '((defun    2 2)
           (defmacro 2 2)
           (funcall  1 4)
           (if       1 4)
           (lambda   1 8)
           (cond     1 6)
           (let      1 4)
           (let*     1 4)
           (list     1 1))))
    ((lambda (loop) (loop loop form 0))
     (lambda (loop form indent)
       (common::spaces indent stream)
       (cond
         ((consp form)
          (cond
            ((symbolp (car form))
               (let ((indent-desc (assoc (car form) pprint-indents)))
                 (if indent-desc
                     (cond
                       ((eq (nth 0 indent-desc) 1) 
                          (common:format stream "(~A ~A~%" (nth 0 form) (nth 1 form))
                          (mapc (lambda (el) (loop loop el (fixnum+ indent (nth 1 indent-desc)))) (nthcdr 2 form))
                          (common:format stream ")"))
                       ((eq (nth 0 indent-desc) 2) 
                          (common:format stream "(~A ~A ~A~%" (nth 0 form) (nth 1 form) (nth 2 form))
                          (mapc (lambda (el) (loop loop el (fixnum+ indent (nth 1 indent-desc)))) (nthcdr 3 form))
                          (common:format stream ")"))
                       (t (core:warn form "pprint botch")))
                     (progn
                       (common:format stream "(")
                       (mapc (lambda (el) (loop loop el 1)) form)
                       (common:format stream ")")))))
             (t (common:format stream "~A~%" form))))
          (t (common:format stream "~A" form)))))
    ()))

;;;
;;; common format
;;;
#|
(defun format (stream fmt-string &rest args)
  (errorp-unless stringp fmt-string "format not a string")
  (let*
     ((dest
        (cond
          ((null stream) (make-string-stream :output ""))
          ((eq stream t) ())
          ((streamp stream) stream)
          (t (error-if t stream "format not a stream designator"))))
      (fmt (make-string-stream :input fmt-string))
      (radix-string (lambda (radix fix)
        (let ((out (make-string-stream :output ""))
              (digits "0123456789abcdef"))
          ((lambda (radixf)
             (cond
               ((zerop fix) "0")
               (t (radixf radixf fix) (get-str out))))
           (lambda (fn fx)
             (cond
               ((zerop fx) ())
               (t (fn fn (car (trunc fx radix)))
                   (wr-char (sv-ref digits (logand fx (1- radix))) out)))))))))

     ((lambda (loop) (loop loop args))
      (lambda (loop args)
        (let ((ch (rd-char fmt)))
          (cond
            ((null ch)
               (if stream
                   dest
                   (get-str dest)))
            ((eq ch #\~)
               (let ((dir (rd-char fmt)))
                 (cond
                   ((null dir) (error-if t dir "format eof while processing directive") ())
                   ((eq dir #\~) (wr-char #\~ dest) (loop loop args))
                   ((eq dir #\%) (terpri dest) (loop loop args))
                   (t (cond
                        ((eq dir #\A) (print (car args) dest ()))
                        ((eq dir #\C) (print (car args) dest ()))
                        ((eq dir #\S) (print (car args) dest t))
                        ((eq dir #\I) (pprint (car args) dest))
                        ((eq dir #\X)
                           (let ((fix (car args)))
                             (errorp-unless fixnump fix "format ~X not a fixnum")
                             (print-str (radix-string 16 fix) dest ())))
                         ((eq dir #\D)
                            (let ((fix (car args)))
                              (errorp-unless fixnump fix "format ~D not a fixnum")
                              (print-str (radix-string 10 fix) dest ())))
                         ((eq dir #\O)
                            (let ((fix (car args)))
                              (errorp-unless fixnump fix "format ~O not a fixnum")
                              (print-str (radix-string 8 fix) dest ())))
                         ((eq dir #\W) (print (car args) dest t))
                         ((eq dir #\a) (print-str (car args) dest ()))
                         ((eq dir #\s) (print-str (car args) dest t))
                         (t (error-if t dir "unrecognized format directive")))
                        (loop loop (cdr args))))))
                (t (wr-char ch dest) (loop loop args))))))))
|#

