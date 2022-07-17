;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;;  metrics: mu metrics
;;;
(defmacro time (form verbosity)
  (let* ((form (core:compile form))
         (zip-2
           (lambda (list-1 list-2)
             (let ((zip- (lambda (fn l1 l2)
               (if (or (core:null l1) (null l2))
                   ()
                   (cons (cons (car l1) (car l2)) (fn fn (cdr l1) (cdr l2)))))))
               (zip- zip- list-1 list-2))))
           (heap
             (lambda ()
               (list
                 (list 'conses (env:hp-info :heap :cons))
                 (list 'except (env:hp-info :heap :except))
                 (list 'functions (env:hp-info :heap :func))
                 (list 'streams (env:hp-info :heap :stream))
                 (list 'symbols (env:hp-info :heap :symbol))
                 (list 'vectors (env:hp-info :heap :vector)))))
           (pr-time
              (lambda (start stop before after)
                (when verbosity
                      (format :t "time:    system     ~Ams    process  ~Ams~%"
                        (fixnum- (svref stop 1) (svref start 1))
                        (fixnum- (svref stop 2) (svref start 2)))
                      (mapc
                        (lambda (desc)
                          (let ((type (nth 0 (car desc)))
                                (before (nth 1 (car desc)))
                                (after (nth 1 (cdr desc))))
                            (unless (eq (svref after 2) (svref before 2))
                              (format :t "~A:    bytes    ~A    consumed ~A~%"
                                type
                                (fixnum- (svref after 1) (svref before 1))
                                (fixnum- (svref after 2) (svref before 2))))))
                        (zip-2 before after)))
                  (unless verbosity
                    (format :t "~A ~A"
                      (fixnum- (svref stop 1) (svref start 1))
                      (fixnum- (svref stop 2) (svref start 2)))
                    (core:mapc
                      (lambda (desc)
                        (let ((before (nth 1 (car desc)))
                              (after (nth 1 (cdr desc))))
                          (format :t " ~A ~A"
                            (fixnum- (svref after 1) (svref before 1))
                            (fixnum- (svref after 2) (svref before 2)))))
                      (zip-2 before after))
                      (core:terpri mu:std-out)
                    ))))
       (let ((before (heap))
                  (start (system:utimes))
                  (value (eval form))
                  (stop (system:utimes))
                  (after (heap)))
         (pr-time start stop before after)
         (list :quote value))))

(defun room (verbosity)
  (let ((heap (list
                (list 'conses (env:hp-info :heap :cons))
                (list 'except (env:hp-info :heap :except))
                (list 'functions (env:hp-info :heap :func))
                (list 'streams (env:hp-info :heap :stream))
                (list 'symbols (env:hp-info :heap :symbol))
                (list 'vectors (env:hp-info :heap :vector)))))
    (mapc
      (lambda (desc)
        (let ((type (nth 0 desc))
              (info (nth 1 desc)))
              (format :t "~A:  bytes ~A consumed ~A~%"
                               type
                               (svref info 1)
                               (svref info 2))))
      heap)
    verbosity))
