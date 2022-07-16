;:func:fixnum:func:func:func:func:func109876543210-1
(mu:write (mu:type-of (let ((foo (lambda () 1))) foo)) mu:std-out ());:func
(mu:write (mu:type-of (let ((foo (lambda () 1))) (foo))) mu:std-out ());:fixnum
(mu:write (mu:type-of (letf ((foo () foo)) foo)) mu:std-out ());:func
(mu:write (mu:type-of (letf ((foo () foo)) (funcall foo))) mu:std-out ());:func
(mu:write (mu:type-of (letf ((foo () foo)) (foo))) mu:std-out ());:func
(mu:write (mu:type-of (letf* ((a-func () a-func)) a-func)) mu:std-out ());:func
(mu:write (mu:type-of (letf ((foo (sym) sym)) (foo foo))) mu:std-out ());:func
(mu:write (letf ((count-down (n) (mu:write n mu:std-out :t) (if (zerop n) -1 (count-down (1- n))))) (count-down 10)) mu:std-out ());109876543210
