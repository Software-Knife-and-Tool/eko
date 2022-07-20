
#
# performance suite
#
.PHONY: help perf prep prof report base summary valgrind clean

SHELL=/bin/bash
NTESTS ?= 5
LLVMTOOLSDIR = /System/Volumes/Data/Library/Developer/CommandLineTools/usr/bin

help:
	@echo "--- performance suite"
	@echo "    base - save current perf values"
	@echo "    perf - run performance tests"
	@echo "    prep - create local distribution for tests"
	@echo "    prof - generate profile report (requires llvm-profdata)"
	@echo "    report - display perf report"
	@echo "    summary - generate perf report"
	@echo "    valgrind - generate valgrind reports (requires valgrind)"
	@echo "    clean - remove all temporary files"

prep:
	@rm -rf ./eko
	@tar xfz ../dist/eko-0.0.1.tgz

base:
	@NTESTS=10 make perf
	@cp mu-`date +"%m-%d-%y"`.perf mu-base.perf
	@cp core-`date +"%m-%d-%y"`.perf core-base.perf
	@cp nil-`date +"%m-%d-%y"`.perf nil-base.perf
	@cp mapc-`date +"%m-%d-%y"`.perf mapc-base.perf
	@cp forms-0-`date +"%m-%d-%y"`.perf forms-0-base.perf
	@cp forms-1-`date +"%m-%d-%y"`.perf forms-1-base.perf
	@cp forms-2-`date +"%m-%d-%y"`.perf forms-2-base.perf
	@cp forms-3-`date +"%m-%d-%y"`.perf forms-3-base.perf
	@cp letf-`date +"%m-%d-%y"`.perf letf-base.perf
	@cp flet-`date +"%m-%d-%y"`.perf flet-base.perf

perf: prep
	@rm -f *.out
	@for run in {1..$(NTESTS)}; do		\
           (source ./preface-test.sh "(preface:time () ())" >> nil.out ); 						\
           (source ./preface-test.sh "(preface:time (mu:keysymp :t) ())" >> mu.out );					\
           (source ./preface-test.sh "(preface:time (core:funcall mu:fixnum+ '(1 2)) ())" >> core.out ); 		\
           (source ./preface-test.sh "(preface:time (core:mapc core:null ''(1 2 3 4 5 6)) ())" >> mapc.out ); 		\
           (source ./preface-test.sh '(preface:time (core:load "./preface:forms-0.l" () ()) ())' >> forms-0.out ); 	\
           (source ./preface-test.sh '(preface:time (core:load "./preface:forms-1.l" () ()) ())' >> forms-1.out ); 	\
           (source ./preface-test.sh '(preface:time (core:load "./preface:forms-2.l" () ()) ())' >> forms-2.out ); 	\
           (source ./preface-test.sh '(preface:time (core:load "./preface:forms-3.l" () ()) ())' >> forms-3.out ); 	\
           (source ./preface-test.sh '(preface:time (core:load "./preface:flet.l" () ()) ())' >> flet.out ); 		\
           (source ./preface-test.sh '(preface:time (core:load "./preface:letf.l" () ()) ())' >> letf.out );		\
	done

	@python3 mean.py mu.out > mu-`date +"%m-%d-%y"`.perf 
	@python3 mean.py core.out > core-`date +"%m-%d-%y"`.perf 
	@python3 mean.py mapc.out > mapc-`date +"%m-%d-%y"`.perf
	@python3 mean.py forms-0.out > forms-0-`date +"%m-%d-%y"`.perf
	@python3 mean.py forms-1.out > forms-1-`date +"%m-%d-%y"`.perf
	@python3 mean.py forms-2.out > forms-2-`date +"%m-%d-%y"`.perf
	@python3 mean.py forms-3.out > forms-3-`date +"%m-%d-%y"`.perf
	@python3 mean.py flet.out > flet-`date +"%m-%d-%y"`.perf
	@python3 mean.py letf.out > letf-`date +"%m-%d-%y"`.perf
	@python3 mean.py nil.out > nil-`date +"%m-%d-%y"`.perf 

report:
	@echo ';;; per test average times (microseconds)'
	@echo ';;;' nil-`date +"%m-%d-%y"`.perf vs nil-base.perf
	@diff  nil-`date +"%m-%d-%y"`.perf nil-base.perf | grep -v -- --- | egrep -v '^[123456789]'
	@echo ';;;' mu-`date +"%m-%d-%y"`.perf vs mu-base.perf
	@diff mu-`date +"%m-%d-%y"`.perf mu-base.perf | grep -v -- --- | egrep -v '^[123456789]'
	@echo ';;;' core-`date +"%m-%d-%y"`.perf vs core-base.perf	
	@diff core-`date +"%m-%d-%y"`.perf core-base.perf | grep -v -- --- | egrep -v '^[123456789]'
	@echo ';;;' mapc-`date +"%m-%d-%y"`.perf vs mapc-base.perf
	@diff mapc-`date +"%m-%d-%y"`.perf mapc-base.perf | grep -v -- --- | egrep -v '^[123456789]'
	@echo ';;;' forms-0-`date +"%m-%d-%y"`.perf vs forms-0-base.perf
	@diff forms-0-`date +"%m-%d-%y"`.perf forms-0-base.perf | grep -v -- --- | egrep -v '^[123456789]'
	@echo ';;;' forms-1-`date +"%m-%d-%y"`.perf vs forms-1-base.perf
	@diff forms-1-`date +"%m-%d-%y"`.perf forms-1-base.perf | grep -v -- --- | egrep -v '^[123456789]'
	@echo ';;;' forms-2-`date +"%m-%d-%y"`.perf vs forms-2-base.perf
	@diff forms-2-`date +"%m-%d-%y"`.perf forms-2-base.perf | grep -v -- --- | egrep -v '^[123456789]'
	@echo ';;;' forms-3-`date +"%m-%d-%y"`.perf vs forms-3-base.perf
	@diff forms-3-`date +"%m-%d-%y"`.perf forms-3-base.perf | grep -v -- --- | egrep -v '^[123456789]'
	@echo ';;;' flet-`date +"%m-%d-%y"`.perf vs flet-base.perf
	@diff flet-`date +"%m-%d-%y"`.perf flet-base.perf | grep -v -- --- | egrep -v '^[123456789]'
	@echo ';;;' letf-`date +"%m-%d-%y"`.perf vs letf-base.perf
	@diff letf-`date +"%m-%d-%y"`.perf letf-base.perf | grep -v -- --- | egrep -v '^[123456789]'
	@echo
	@echo ';;; per test object consumption'
	@for i in nil mu core mapc forms-0 forms-1 forms-2 forms-3 letf flet; do	\
	    python3 summary.py $$i $$i.out;				\
	done

prof: prep
	@bash ./preface-test.sh '(preface:time (core:load "./preface:forms-2.l" () ()) ())'
	@$(LLVMTOOLSDIR)/llvm-profdata show --all-functions --topn=25 default.profraw > profile.report 2>&1

valgrind: memcheck callgrind cachegrind

memcheck: prep
	@valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=10 \
		--leak-resolution=med --show-leak-kinds=definite \
		./eko/bin/mu-runtime                                           \
		    -l ./eko/eko/eko:core.l	       	                       \
		    -q '(core:load "./eko/eko/eko:preface.l" () ())'       \
		    -q "(core:eval (core:compile '(defconst preface::require-path \"./eko/lib-modules\")))" \
		    -q "(core:eval (core:compile '(preface:time (core:load \"./preface:forms-2.l\" () ()) ())))" > memcheck.report 2>&1 || true

callgrind: prep
	@valgrind --tool=callgrind 	\
		./eko/bin/mu-runtime                                           \
		    -l ./eko/eko/eko:core.l	       	                       \
		    -q '(core:load "./eko/eko/eko:preface.l" () ())'       \
		    -q "(core:eval (core:compile '(defconst preface::require-path \"./eko/lib-modules\")))" \
		    -q "(core:eval (core:compile '(preface:time (core:load \"./preface:forms-2.l\" () ()) ())))" > callgrind.report 2>&1 || true

	@callgrind_annotate --auto=yes callgrind.out.* >> callgrind.report
	@rm -f callgrind.out.*

cachegrind: prep
	@valgrind --tool=cachegrind	\
	./eko/bin/mu-runtime                                           	\
		    -l ./eko/eko/eko:core.l	       	                \
		    -q '(core:load "./eko/eko/eko:preface.l" () ())'    \
		    -q "(core:eval (core:compile '(defconst preface::require-path \"./eko/lib-modules\")))" \
		    -q "(core:eval (core:compile '(preface:time (core:load \"./preface:forms-2.l\" () ()) ())))" > cachegrind.report 2>&1 || true

clean:
	@rm -rf *-*-*-* *.out *.profraw callgrind.out.* cachegrind.out.* cachegrind.out eko
