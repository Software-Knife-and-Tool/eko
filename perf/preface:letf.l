(letf ((foo (n) (cond ((zerop n) 0) (t (foo (1- n)))))) (foo 500))
