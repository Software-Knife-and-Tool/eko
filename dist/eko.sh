#! /bin/bash
BASE=${EKO_HOME:=/opt/eko}
$BASE/bin/mu-runtime $BASE/eko/eko:core.l $BASE/eko/eko.l "$@"
