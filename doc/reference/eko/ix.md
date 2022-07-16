---
title: print library
---

`@comment_type ;;; %s
@title Appendix - ix Forms

@s ix Forms
<hr><br/>
<p></p>
*ix* forms are implemented on top of the *core* library.

--- ix forms
(defconst ix:version "0.0.1")

(defun ix:apply (fn &rest lists)
  (assert functionp fn "ix:apply requires a function")
  ((lambda (loop) (core:funcall fn (loop loop lists)))
   (lambda (loop args)
     (if (cdr args)
         (cons (car args) (loop loop (cdr args)))
         ((lambda ()
           (assert listp (car args) "ix:apply requires a list")
           (car args)))))))

(defun ix:funcall (fn &rest args) (core:funcall fn args))

(defun ix:append (&rest lists)
  (foldr
    (lambda (el acc)
      (if (listp el)
          (foldr (lambda (el acc) (cons el acc)) acc el)
          el))
      ()
      lists))

(defun ix:vector (&rest elements) (list-sv :t elements))

(defun ix:list (&rest list) list)

(defun ix:list* (&rest args)
  (if (cdr args)
      (cons (car args) (core:funcall list* (cdr args)))
      (car args)))

(defmacro ix:progn (&rest body)
  (list (list* 'lambda () body)))

(defmacro ix:when (test &rest body)
  (ix:list 'if test (list* 'ix:progn body)))
(defmacro ix:unless (test &rest body)
  (ix:list 'if test () (list* 'ix:progn body)))

(defmacro ix:let (binds &rest body)
  (assert core:listp binds "ix:let requires a list")
  (list*
    (list* 'lambda (mapcar car binds) body)
    (mapcar (lambda (el) (nth 1 el)) binds)))

(defmacro ix:let* (binds &rest body)
  ((lambda (fn)
     (if binds
       (fn fn binds)
       (list* 'ix:progn body)))
   (lambda (fn binds)
     (if (cdr binds)
         (list 'ix:let (list (car binds)) (core:funcall fn (list fn (cdr binds))))
         (list* 'ix:let (list (car binds)) body)))))

(defun ix::reclose (closure)
  (assert functionp closure "ix::reclose requires a function")
  (let* ((closed (core::lambda-closure (core::fn-lambda closure)))
         (stanza (car closed)))
    (assert (lambda (closed) (eq (length closed) 1)) closed "reclose botch")
    (list (cons (car stanza) (vector closure)))))

(defmacro ix:recur (name lambda &rest body)
  (list 'let*
        (list (list 'closure
                    (list 'let
                          (list (list name (list 'lambda ())))
                                (list* 'lambda lambda body)))
              (list 'desc (list 'core::fn-lambda 'closure))
              (list 'closed (list 'ix::reclose 'closure))
              (list 'clonefn
                    (list 'core::clone
                          'closure
                          (list :quote name)
                          (list 'core::lambda-descriptor
                                (list 'core::lambda-reqs 'desc)
                                (list 'core::lambda-rest 'desc)
                                (list 'core::lambda-macrop 'desc)
                                (list 'core::lambda-env 'desc)
                                'closed)
                          (list 'core::fn-form 'closure))))
        'clonefn))

(defconst ix:gensym
  (let ((genstr (st-ostr "0")))
    (lambda ()
      ((lambda (nstr)
         ((lambda (n gsym ) 
            (print (fixnum+ 1 n) genstr ())
            (symbol gsym))
          (mu:read (st-istr nstr))
          (string-append "g" nstr)))
       (get-str genstr)))))

(defmacro ix:cond (&rest clauses)
  (car
    (foldr
      (lambda (clause cond)
        ;;; (form) or (form . body)
        (assert consp clause "core:cond requires a cons")
        (let ((test (car clause)) (body (cdr clause)))
          (if body
            (list (list* 'if test (list* 'ix:progn body) cond))
            (list
              (let ((gsym (gensym)))
                (list 'ix:let (list (list gsym test))
                  (list* 'if gsym gsym cond)))))))
    ()
    clauses)))

(defmacro ix:and (&rest terms)
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

(defmacro ix:or (&rest terms)
  (foldr
    (lambda (cons acc)
      (let ((term (car cons)))
        (if (cdr cons)
            (let ((gsym (gensym)))
              (list*
                (list
                  'ix:let (list (list gsym (car cons)))
                  (list 'if gsym gsym acc))))
         term)))
    ()
    (maplist identity terms)))

(defun ix:eql (x y)
   (cond
     ((eq x y) t)
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
---

--- ix letf --- noWeave
(defmacro ix:letf (binds &rest body)
  (assert listp binds "ix:letf requires a list")
  (list* 'ix:let
         (mapcar
           (lambda (bind)
             (list (car bind)
                   (core:funcall
                     ix:recur
                     (list*
                       (nth 0 bind)
                       (nth 1 bind)
                       (nthcdr 2 bind)))))
             binds)
         body))

(defmacro ix:letf* (binds &rest body)
  (assert listp binds "ix:letf* requires a list")
  ((lambda (fn)
     (if binds
       (fn fn binds)
       (list* 'ix:progn body)))
   (lambda (fn binds)
     (if (cdr binds)
         (list 'let (list (car binds)) (core:funcall fn (list fn (cdr binds))))
         (list* 'let (list (car binds)) body)))))
---

--- ix-forms --- noWeave
@{ix forms}
@{ix letf}
---

--- ix:ix.l --- noWeave
(mu:ns "ix")
@{ix-forms}
---
