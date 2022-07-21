(flet ((foo (foo n) (cond ((zerop n) 0) (t (foo foo (1- n)))))) (foo foo 5))
