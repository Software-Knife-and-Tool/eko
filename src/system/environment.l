;;;  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam (putnamjm.design@gmail.com)
;;;  SPDX-License-Identifier: MIT

(defun system:current-working-directory ()
  (system:cwd))

(defun system:chdir (path)
  (errorp-unless stringp path "system:chdir not a string")
  (system::chdir path))

(defun system:exit (rc)
  (errorp-unless fixnump rc "system:exit not a fixnum")
  (system:exit rc))

(defun system:logname ()
  (system:logname))

(defun system:process-environment ()
  (system:procenv))

(defun system:system (cmd)
  (errorp-unless stringp cmd "system:system not a string")
  (system:system cmd))

(defun system:utimes ()
    (system:utimes))
