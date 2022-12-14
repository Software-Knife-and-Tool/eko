;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; core streams
;;;
(mu::intern core::ns :intern "write-stream-designator"
  (:lambda (designator)
    (:if (core:null designator)
         mu:std-out
         (:if (core:streamp designator)
              designator
              (core:errorp-unless core:streamp designator "core::write-stream-designator: not a stream")))))

(mu::intern core::ns :intern "read-stream-designator"
   (:lambda (designator)
      (:if (core:null designator)
         mu:std-in
         (:if (mu:eq :t designator)
              mu:std-in
              (:if (core:streamp designator)
                   designator
                   (core:errorp-unless core:streamp designator "core::read-stream-designator: not a stream"))))))

(mu::intern core::ns :extern "make-string-stream"
  (:lambda (dir init)
    (core:errorp-unless core:keywordp dir "core:make-string-stream: not a direction keyword")
    (core:errorp-unless core:stringp init "core:make-string-stream: not a string initializer")
    (:if (mu:eq dir :input)
         (mu:open :string :input init)
         (:if (mu:eq dir :output)
              (mu:open :string :output init)
              (core:error dir "core:make-string-stream: not a direction keyword")))))

(mu::intern core::ns :extern "get-output-string-stream" mu:get-str)

(mu::intern core::ns :extern "eofp"
   (:lambda (designator)
      (mu:eofp (core::read-stream-designator designator))))

(mu::intern core::ns :extern "open"
   (:lambda (dir path)
     (core:errorp-unless core:stringp path "core:open: not a path string")
     (core:errorp-unless core:keywordp dir "core:open: not a direction keyword")
     (:if (mu:eq dir :input)
          (mu:open :file :input path)
          (:if (mu:eq dir :output)
               (mu:open :file :output path)
               (core:error dir "core:open: not a direction keyword")))))

(mu::intern core::ns :extern "close" mu:close)

(mu::intern core::ns :extern "write-char"
   (:lambda (char designator)
     (mu:wr-char (core::write-stream-designator designator))))  

(mu::intern core::ns :extern "write-byte"
   (:lambda (byte designator)
     (mu:wr-byte byte (core::write-stream-designator designator))))

(mu::intern core::ns :extern "write"
   (:lambda (object designator escape)
     (mu:write object (core::write-stream-designator designator) escape)))

(mu::intern core::ns :extern "terpri"
   (:lambda (designator)
     (core:write-byte #xa designator)))

(mu::intern core::ns :extern "read-char"
   (:lambda (designator error-eofp eof-value)
     ((:lambda (stream)
        (:if (mu:eofp stream)
             (:if error-eofp
                  (core:error stream "core:read-char: end of file")
                  eof-value)
             (mu:rd-char stream)))
      (core::read-stream-designator designator))))  

(mu::intern core::ns :extern "read-byte"
   (:lambda (designator error-eofp eof-value)
     ((:lambda (stream)
        (:if (mu:eofp stream)
             (:if error-eofp
                  (core:error stream "core:read-char: end of file")
                  eof-value)
             (mu:rd-byte stream)))
      (core::read-stream-designator designator))))

(mu::intern core::ns :extern "unread-char"
   (:lambda (char designator)
     (mu:un-char (core::write-stream-designator designator))))  

(mu::intern core::ns :extern "unread-byte"
   (:lambda (char designator)
     (mu:un-byte (core::write-stream-designator designator))))

(mu::intern core::ns :extern "read"
   (:lambda (designator eof-errorp eof-value)
     ((:lambda (stream)
        (:if (mu:eofp stream)
             (:if eof-errorp
                  (core:error stream "core:read: unexpected end of file")
                  eof-value)
             ((:lambda (form)
                (:if (mu:eq form core::read-list-eol)
                     (core:error stream "core:read: unmatched close parenthesis")
                     form))
              (core::read stream))))
      (core::read-stream-designator designator))))

