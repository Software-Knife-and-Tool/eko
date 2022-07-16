;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;; sequences
;;;
(mu::intern core::ns :extern "length"
   (:lambda (seq)
      (core:errorp-unless core:sequencep seq "core:length not a sequence")
      (:if (core:listp seq)
           (mu:length seq)
           (mu:sv-length seq))))

(mu::intern core::ns :extern "elt"
   (:lambda (seq nth)
      (core:errorp-unless core:sequencep seq "core:elt: not a sequence")
      (core:errorp-unless core:fixnump nth "core:elt: not a fixnum")
      (:if (core:listp seq)
           (mu:nth nth seq)
           (mu:sv-ref seq nth))))

;;;
;;; vectors
;;;
(mu::intern core::ns :extern "vector-type"
   (:lambda (vector)
     (core:errorp-unless core:vectorp vector "core:vector-type: not a vector")
     (mu:sv-type vector)))

(mu::intern core::ns :extern "svref"
   (:lambda (vector nth)
     (core:errorp-unless core:vectorp vector "core:svref: not a vector")
     (core:errorp-unless core:fixnump nth "core:svref: not a fixnum")
     (mu:sv-ref vector nth)))

;;;
;;; folds
;;;
(mu::intern core::ns :extern "foldl"
   (:lambda (fn init seq)
      (core:errorp-unless core:functionp fn "core:foldl not a function")
      (core:errorp-unless core:sequencep seq "core:foldl not a sequence")
      (:if (core:listp seq)
         ((:lambda ()
             ((:lambda (fold) (fold fold init seq))
              (:lambda (fold acc seq)
                 (:if (core:null seq)
                      acc
                      (fold fold (fn (mu:car seq) acc) (mu:cdr seq)))))))
         ((:lambda ()
             ((:lambda (fold) (fold fold init 0))
              (:lambda (fold acc index)
                 (:if (mu:fixnum< index (mu:sv-len seq))
                      (fold fold (fn (mu:sv-ref seq index) acc) (core:1+ index))
                      acc))))))))

(mu::intern core::ns :extern "foldr"
   (:lambda (fn init seq)
      (core:errorp-unless core:functionp fn "core:foldr not a function")
      (core:errorp-unless core:sequencep seq "core:foldr not a sequence")
      (:if (core:listp seq)
           ((:lambda (fold) (fold fold seq))
            (:lambda (fold seq)
               (:if (core:null seq)
                    init
                    (fn (mu:car seq) (fold fold (mu:cdr seq))))))
           ((:lambda (fold) (fold fold init (core:1- (mu:sv-len seq))))
            (:lambda (fold acc index)
               (:if (mu:fixnum< -1 index)
                    (fold fold (fn (mu:sv-ref seq index) acc) (core:1- index))
                    acc))))))

;;;
;;; finds
;;;
(mu::intern core::ns :extern "findl-if"
   (:lambda (fn seq)
      (core:errorp-unless core:functionp fn "core:findl-if: not a function")
      (core:errorp-unless core:sequencep seq "core:findl-if: not a sequence")
      (core:foldl
        (:lambda (el acc) (:if (fn el) el acc))
        ()
        seq)))

(mu::intern core::ns :extern "findr-if"
   (:lambda (fn list)
      (core:errorp-unless core:functionp predicate "core:findr-if not a function")
      (core:errorp-unless core:sequencep list "core:findr-if not a sequence")
      (core:foldr
        (:lambda (el acc) (:if (fn el) el acc))
        ()
        list)))

;;;
;;; position
;;;
(mu::intern core::ns :extern "positionl"
   (:lambda (item seq)
      (core:errorp-unless core:sequencep seq "core:positionl: not a sequence")
      (:if (core:listp seq)
           ((:lambda (fold) (fold fold 0 seq))
            (:lambda (fold index seq)
               (:if (mu:fixnum< index (mu:length seq))
                    (:if (mu:eq item (mu:car seq))
                         index   
                         (fold fold (core:1+ index) (mu:cdr seq)))
                    ())))
           ((:lambda (fold) (fold fold 0))
            (:lambda (fold index)
               (:if (mu:fixnum< index (mu:sv-len seq))
                    (:if (mu:eq (mu:sv-ref seq index) item)
                         index   
                         (fold fold (core:1+ index)))
                    ()))))))

(mu::intern core::ns :extern "positionr"
   (:lambda (item seq)
      (core:errorp-unless core:sequencep seq "core:positionr: not a sequence")
      (:if (core:listp seq)
         ((:lambda (fold) (fold fold (mu:length seq)))
          (:lambda (fold index)
              (:if (core:zerop index)
                 (:if (mu:eq item (mu:nth (core:1- seq) seq))
                      index   
                      (fold fold (core:1- index)))
                  ())))
         ((:lambda (fold) (fold fold (mu:sv-len seq)))
          (:lambda (fold index)
             (:if (core:zerop index)
                  ()
                  (:if (mu:eq (mu:sv-ref seq (core:1- index)) item)
                       (core:1- index)
                       (fold fold (core:1- index)))))))))

