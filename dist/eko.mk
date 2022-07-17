#
# eko makefile
#
SRC = ../src

CORE = \
	core.l	     	\
        compile.l    	\
        eval.l	     	\
        exceptions.l 	\
        functions.l  	\
        format.l     	\
        lambda.l     	\
        lists.l      	\
        load.l       	\
        macro.l      	\
        quasiquote.l 	\
        read.l       	\
        sequences.l  	\
        streams.l    	\
        strings.l

PREFACE = \
	preface.l   	\
	common.l   	\
	compile.l	\
	describe.l 	\
	lists.l	    	\
	metrics.l	\
	repl.l	    	\
	require.l   	\
	state.l	    	\
        environment.l	\
        print.l

eko:
	@(cd $(SRC)/core ; cat $(CORE)) > eko:core.l
	@(cd $(SRC)/preface; cat $(PREFACE)) > eko:preface.l

clean:
	@rm -f eko:core.l eko:preface.l
