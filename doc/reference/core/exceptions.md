---
title: print library
---

@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Exceptions

@s Summary
<p></p>
We'll need some utilities to supplement the code generation functions in *mu.l*.

@s Exceptions
<p></p>
For reporting errors and conditions of various kinds.

--- core exceptions
(:const core:warn
   (:lambda (obj msg)
     (core:print-str "warn: " () ())
     (core:print-str msg () ())
     (core:print-str " " () ())
     (mu:print obj () :t)
     (mu:terpri ())
     obj))
  
(:const core:error
   (:lambda (obj msg)
     (core:print-str "error: " () ())
     (core:print-str msg () ())
     (core:print-str ": " () ())
     (mu:print obj () :t)
     (mu:terpri ())
     (mu:raise (mu:except :mu-err :error :type obj))))

(:const core:assert
   (:lambda (testfn obj msg)
      (:if (testfn obj)
         obj
         ((:lambda ()
             (core:print-str "assert: " () ())
             (core:print-str msg () ())
             (core:print-str ": " () ())
             (mu:print obj () ())
             (mu:terpri ())
         (mu:raise (mu:except :mu-asrt :error :type obj)))))))

(:const core:check-type
   (:lambda (obj type msg)     
     (:if (mu:eq (mu:type-of obj) type)
       obj
       ((:lambda ()
          (core:print-str "check-type: " () ())
          (core:print-str msg () ())
          (core:print-str ": " () ())
          (mu:print obj () ())
          (mu:terpri ())
          (mu:raise (mu:except :mu-chk :error :type obj)))))))

(:const core:print-except
   (:lambda (ex by)
      (core:print-str ";;; exception raised by " () ())
      (core:print-str by () ())
      (core:print-str " with " () ())
      (mu:print (mu::view ex) () ())
      (mu:terpri ())
      (core:print-str ";;;     in " () ())
      (mu:print (mu::view ex) () ())
      (mu:terpri ())))
---

@s Break
<hr>
<br/>
`core:break` reads forms from *std-in*, compiles them, evaluates them, and prints the results.

<p></p>
<br/>

***core:break*** *exception* => *boolean*

<p></p>
<br/>

`arguments:`
>
> `exception:` exception designator
>

`returns:` *:t* or *:nil*

<p></p>
<br/>

`Exceptions:`
>
> `type:` if `exception` is not a exception designator.
>

--- core-break
(:const core::break
   (:lambda (cmd ex)
      (:if (mu:eq cmd :help)
         ((:lambda ()
             (core:print-str "break help:" () ()) (mu:terpri ())
             (core:print-str ":e - exit process" () ()) (mu:terpri ())
             (core:print-str ":r - return from break" () ()) (mu:terpri ())
             (core:print-str ":s - print stack frames" () ())
             (mu:terpri ())))
         (:if (mu:eq cmd :e)
            (platform:exit 0)
            (:if (mu:eq cmd :s)
                 (:lambda ()
                    (core:mapc
                      (:lambda (frame)
                         (mu:print frame () ())
                         (mu:terpri ()))
                      (mu::ev-stk))
                   ())
                 (:if (mu:eq cmd :r)
                      :return
                      ()))))))

(:const core:break
   (:lambda (except)
      ((:lambda (ifs)
          (core:check-type ifs :stream "cannot open input stream (core:break)")
          ((:lambda (loop)
              (core:print-str ";;; entering break loop, :help for commands" () ()) (mu:terpri ())
              (core:print-str "break> " () ())
              (loop loop (mu:read ifs)))
           (:lambda (loop form)
              ((:lambda (exit)
                  (:if exit
                     ()
                     ((:lambda ()
                         (mu:print (core:eval (core:compile form ())) () :t)
                         (mu:terpri ())
                         (core:print-str "break> " () ())
                         (loop loop (mu:read ifs))))))
                 (:if (mu:keyp form)
                    (core::break form except)
                    ())))))
       mu:std-in)))
---

--- core-exceptions --- noWeave
@{core exceptions}
@{core-break}
---

--- core:exceptions.l --- noWeave
@{core-exceptions}
---
