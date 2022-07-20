;:func:fixnum:func:func:func:func:func:t
(mu:write (mu:type-of (let ((foo (lambda () 1))) foo)) mu:std-out ());:func
(mu:write (mu:type-of (let ((foo (lambda () 1))) (foo))) mu:std-out ());:fixnum
(mu:write (mu:type-of (letf ((foo () foo)) foo)) mu:std-out ());:func
(mu:write (mu:type-of (letf ((foo () foo)) (funcall foo))) mu:std-out ());:func
(mu:write (mu:type-of (letf ((foo () foo)) (foo))) mu:std-out ());:func
(mu:write (mu:type-of (letf* ((a-func () a-func)) a-func)) mu:std-out ());:func
(mu:write (mu:type-of (letf ((foo (sym) sym)) (foo foo))) mu:std-out ());:func
(mu:write (zerop (letf ((foo (n) (cond ((zerop n) 0) (t (foo (1- n)))))) (foo 5))) mu:std-out ());:t
