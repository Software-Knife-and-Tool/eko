---
title: print library
---

@code_type mu .l
@comment_type ;;; %s
@title Appendix - Core Streams

@s Loader
<hr><br/>
`core:loader` reads forms from the given stream, compiles them, and evaluates them.

<p></p>
<br/>

***core:loader*** *stream* => *boolean*

<p></p>
<br/>

`arguments:`
>
> `file:` input file path designator
>
> `stream:` input stream designator
>

`returns:` *:t* or *:nil*

<p></p>
<br/>

`Exceptions:`
>
> `type:` if `file` or `stream` are not designators.
>

--- streams --- noWeave
(:const core:streamp (:lambda (t) (mu:eq :stream (mu:type-of t))))
(:const core::stream-designator
   (:lambda (designator)
      (:if (mu:eq () designator)
         mu:std-out
         (:if (mu:eq :t designator)
            mu:std-in
            (core:assert core:streamp designator "core::stream-designator requires a stream")))))

(:const core:make-string-stream
   (:lambda (dir init)
      (core:assert mu:keyp dir "core:make-string-stream requires a direction keyword")
      (core:assert core:stringp init "core:make-string-stream requires a string initializer")
      (:if (mu:eq dir :input)
           (mu:st-istr init)
           (:if (mu:eq dir :output)
                (mu:st-ostr init)
                (core:error dir "core:make-string-stream requires a direction keyword")))))


(:const core:get-output-string-stream
   (:lambda (stream)
      (mu:get-str stream)))

(:const core:open
   (:lambda (path dir)
      (core:assert core:stringp path "core:open requires a string path")
      (core:assert mu:keyp dir "core:open requires a direction keyword")
      (:if (mu:eq dir :input)
           (mu:st-ifs path)
           (:if (mu:eq dir :output)
                (mu:st-ofs path)
                (core:error dir "core:open requires a direction keyword")))))

(:const core:close
   (:lambda (stream)
      (core:assert core:streamp stream "core:close requires a stream")
      (mu:close stream)))
---

--- core-load
(:const core:load
   (:lambda (source verbose print)
      (core:assert core:stringp source "core:load requires a file designator")
      ((:lambda (ifs)
          (core:check-type ifs :stream "core:load cannot open input file")
          ((:lambda (loop)
              (:if verbose
                 ((:lambda ()
                     (core:print-str (core:string-append ";;; loading " source) () ()) 
                     (mu:terpri ())))
                 ())
              (mu:with-ex
                (:lambda (ex)
                   (:lambda ()
                      (core::print-except ex "core:load")
                      (core:print-str "in file " () ())
                      (core:print-str source () ())
                      (core:print-str "on line " () ())
                      (core:print-str (core::read-clear-line-counter) () ())
                      (mu:terpri ())
                      (core:break ex)))
                   (:lambda ()
                      (loop loop (core:read ifs ()))))
              :t)
           (:lambda (loop form)
              (:if (mu:eofp ifs)
                 :t
                 ((:lambda ()
                     ((:lambda (val)
                         (:if print
                            ((:lambda ()
                                (mu:print val () ())
                                (mu:terpri ())))
                            ()))
                      (core:eval (core:compile form ())))
                     (loop loop (core:read ifs ()))))))))
       (mu:st-ifs source))))
---

--- core-streams --- noWeave
@{streams}
@{core-load}
---

--- core:stream.l --- noWeave
@{core-streams}
---