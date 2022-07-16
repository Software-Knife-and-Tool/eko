;;;
;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

;;;
;;;  eko runtime environment
;;;
(core:load "/opt/eko/eko/eko:preface.l" () ())
(core:eval (core:compile '(defconst preface::require-path "/opt/eko/lib-modules")))
(:if (core:zerop (system:system "stat ./.eko > /dev/null 2>&1"))
     (core:load "./.eko" () ())
     ())
(core:in-namespace (mu:ns "user" (env:find-ns "preface")))
(mu::intern (env:find-ns "preface") :intern "eko-version" "0.0.1")
(preface:repl (env:find-ns "user"))
