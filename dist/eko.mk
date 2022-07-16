#
# eko makefile
#
SRC = ../src

COMMON = \
	common.l   \
	describe.l \
        print.l

COMPILE = \
	compile.l

SYSTEM = \
	system.l   \
        environment.l

METRICS = \
	metrics.l

CORE = \
	core.l	     \
        compile.l    \
        eval.l	     \
        exceptions.l \
        functions.l  \
        format.l     \
        lambda.l     \
        lists.l      \
        load.l       \
        macro.l      \
        quasiquote.l \
        read.l       \
        sequences.l  \
        streams.l    \
        strings.l

PREFACE = \
	preface.l   \
	lists.l	    \
	repl.l	    \
	require.l   \
	state.l	    \

eko:
	@(cd $(SRC)/common ; cat $(COMMON)) > eko:common.l
	@(cd $(SRC)/compile ; cat $(COMPILE)) > eko:compile.l
	@(cd $(SRC)/core ; cat $(CORE)) > eko:core.l
	@(cd $(SRC)/metrics ; cat $(METRICS)) > eko:metrics.l
	@(cd $(SRC)/preface; cat $(PREFACE)) > eko:preface.l
	@(cd $(SRC)/system ; cat $(SYSTEM)) > eko:system.l

clean:
	@rm -f eko:common.l eko:core.l eko:frame.l eko:preface.l eko:repl.l eko:system.l eko:compile.l
