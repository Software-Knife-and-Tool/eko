#! /bin/bash
./eko/bin/mu-runtime                                              \
    -l ./eko/eko/eko:core.l	       	                          \
    -q '(core:load "./eko/eko/eko:preface.l" () ())'             \
    -q "(core:eval (core:compile '(defconst preface::require-path \"./eko/lib-modules\")))" \
    -q "(core:eval (core:compile '(preface:require-lib metrics)))"  \
    -q "(core:eval (core:compile '$*))"
