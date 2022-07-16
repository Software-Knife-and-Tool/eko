;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;;  preface: eko language
;;;
;;;  preface namespace
;;;
(in-namespace (ns "preface" (find-ns "core")))

(defun preface:funcall (fn &rest args)
  (core:funcall fn args))

(defun preface:format (stream fmt-string &rest args)
  (core:format stream fmt-string args))

(defun identity (obj) obj)

(defun apply (fn &rest lists)
  (errorp-unless functionp fn "apply: not a function")
  ((lambda (loop) (core:funcall fn (loop loop lists)))
   (lambda (loop args)
     (if (cdr args)
         (cons (car args) (loop loop (cdr args)))
         ((lambda ()
           (errorp-unless listp (car args) "apply not a list")
           (car args)))))))

(defun append (&rest lists)
  (foldr
    (lambda (el acc)
      (if (listp el)
          (foldr (lambda (el acc) (cons el acc)) acc el)
          el))
      ()
      lists))

(defun vector (&rest elements)
  (list-sv :t elements))

(defun list (&rest list) list)

(defun list* (&rest args)
  (if (cdr args)
      (cons (car args) (core:funcall preface:list* (cdr args)))
      (car args)))

(defmacro progn (&rest body)
  (list (list* 'lambda () body)))

(defmacro when (test &rest body)
  (preface:list 'if test (list* 'progn body)))

(defmacro unless (test &rest body)
  (preface:list 'if test () (list* 'progn body)))

(defmacro let (binds &rest body)
  (errorp-unless core:listp binds "let: not a list")
  (list*
    (list* 'lambda (mapcar car binds) body)
    (mapcar (lambda (el) (nth 1 el)) binds)))

(defmacro let* (binds &rest body)
  ((lambda (fn)
     (if binds
       (fn fn binds)
       (list* 'progn body)))
   (lambda (fn binds)
     (if (cdr binds)
         (list 'let (list (car binds)) (core:funcall fn (list fn (cdr binds))))
         (list* 'let (list (car binds)) body)))))

(defconst gensym
  (let ((genstr (make-string-stream :output "0")))
    (lambda ()
      ((lambda (nstr)
         ((lambda (n gsym ) 
            (core:write (1+ n) genstr ())
            (symbol gsym))
          (mu:read (mu:open :string :input nstr))
          (string-append "g" nstr)))
       (get-str genstr)))))

(defmacro bind-seq (pattern source &rest body)
  (errorp-unless consp pattern "bind-seq: pattern not a list")
  (let* ((source-gsym (gensym))
         (destruct
          (lambda ()
            (let ((nths
                   (foldr
                    (lambda (sym acc)
                      (errorp-unless symbolp sym "bind-seq: not a symbol")
                      (let* ((nth (car acc))
                             (phrases (cdr acc))
                             (phrase (list sym (preface:list 'elt source-gsym nth))))
                        (cons (1- nth) (cons phrase phrases))))
                    (cons (1- (length pattern)) ())
                    pattern)))
              (cdr nths)))))
    (list* 'let* (cons (preface:list source-gsym source) (destruct)) body)))

(defmacro cond (&rest clauses)
  (car
    (foldr
      (lambda (clause cond)
        ;;; (form) or (form . body)
        (errorp-unless consp clause "core:cond not a cons")
        (let ((test (car clause)) (body (cdr clause)))
          (if body
            (list (list* 'if test (list* 'progn body) cond))
            (list
              (let ((gsym (gensym)))
                (list 'let (list (list gsym test))
                  (list* 'if gsym gsym cond)))))))
    ()
    clauses)))

(defmacro and (&rest terms)
  (if terms
    (foldr
      (lambda (cons acc)
        (let ((term (car cons)))
          (if (cdr cons)
              (list 'if term acc)
              term)))
      ()
      (maplist identity terms))
    :t))

(defmacro or (&rest terms)
  (foldr
    (lambda (cons acc)
      (let ((term (car cons)))
        (if (cdr cons)
            (let ((gsym (gensym)))
              (list*
                (list
                  'let (list (list gsym (car cons)))
                  (list 'if gsym gsym acc))))
         term)))
    ()
    (maplist identity terms)))

(defun eql (x y)
   (cond
     ((eq x y) :t)
     ((null (eq (type-of x) (type-of y))) :nil)
     ((stringp x) (string= x y))
     ((and (vectorp x) (vectorp y))
        (and (eq (vector-type x) (vector-type y))
             (eq (vector-length x) (vector-length y))
             (core:funcall eql (vector-to-list x) (vector-to-list y))))
     ((consp x)
        (and (eq (length x) (length y))
             (core:funcall eql (car x) (car y))
             (core:funcall eql (cdr x) (cdr y))))
     (t ())))

(defun preface::reclose (closure)
  (errorp-unless functionp closure ":reclose not a function")
  (let ((closed (core::lambda-closure (core::fn-extension closure))))
    (errorp-unless (lambda (list) (fixnum< 0 (length list))) closed "reclose environment botch")
    (env:fnv-set (car closed) 0 closure))
  closure)

(defmacro recur (name lambda &rest body)
  (let ((gsym (gensym)))
    (list 'let
          (list (list gsym
                      (list 'let
                            (list (list name (list 'lambda lambda)))
                            (list* 'lambda lambda body))))
          (list 'preface::reclose gsym))))

(defmacro letf (binds &rest body)
  (errorp-unless listp binds "letf: not a list")
  (list* 'preface:let
         (mapcar
           (lambda (bind)
             (list (car bind)
                   (core:funcall
                     preface:recur
                     bind)))
             binds)
         body))

(defmacro letf* (binds &rest body)
  (errorp-unless listp binds "letf*: not a list")
  ((lambda (fn)
     (if binds
       (fn fn binds)
       (list* 'progn body)))
   (lambda (fn binds)
     (if (cdr binds)
         (list 'letf (list (car binds)) (core:funcall fn (list fn (cdr binds))))
         (list* 'letf (list (car binds)) body)))))

(defmacro flet (binds &rest body)
  (errorp-unless listp binds "flet: not a list")
  (list* 'let
         (mapcar
          (lambda (bind)
            (list (nth 0 bind)
                  (list* 'lambda (nth 1 bind) (nthcdr 2 bind))))
          binds)
         body))

(defmacro flet* (binds &rest body)
  (errorp-unless listp binds "flet*: not a list")
  ((lambda (fn)
     (if binds
       (fn fn binds)
       (list* 'progn body)))
   (lambda (fn binds)
     (if (cdr binds)
         (list 'flet (list (car binds)) (core:funcall fn (list fn (cdr binds))))
         (list* 'flet (list (car binds)) body)))))

(defun compose (&rest fns)
  (lambda (arg)
    (foldl
     (lambda (fn acc)
       (funcall fn acc))
     arg
     fns)))

(defun complement (fn)
  (compose null fn))
