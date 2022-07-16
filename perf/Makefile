#
# performance suite
#
.PHONY: help perf prep prof report base summary valgrind clean

SHELL=/bin/bash
NTESTS ?= 10
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

perf: prep
	@rm -f mu.out perf.out nil.out core.out mapc.out forms-0.out form-1.out forms-2.out
	@for run in {1..$(NTESTS)}; do                   \
           source ./preface-test.sh "(metrics:time () ())" >> nil.out; \
        done
	@mean.py nil.out > nil-`date +"%m-%d-%y"`.perf 

	@for run in {1..$(NTESTS)}; do                   \
           source ./preface-test.sh "(metrics:time (mu:keysymp :t) ())" >> mu.out; \
        done
	@mean.py mu.out > mu-`date +"%m-%d-%y"`.perf 

	@for run in {1..$(NTESTS)}; do                   \
           source ./preface-test.sh "(metrics:time (core:funcall mu:fixnum+ '(1 2)) ())" >> core.out; \
        done
	@mean.py core.out > core-`date +"%m-%d-%y"`.perf 

	@for run in {1..$(NTESTS)}; do                    \
           source ./preface-test.sh "(metrics:time (core:mapc core:null ''(1 2 3 4 5 6)) ())" >> mapc.out; \
        done
	@mean.py mapc.out > mapc-`date +"%m-%d-%y"`.perf

	@for run in {1..$(NTESTS)}; do                    \
           source ./preface-test.sh '(metrics:time (core:load "./preface:forms-0.l" () ()) ())' >> forms-0.out; \
        done
	@mean.py forms-0.out > forms-0-`date +"%m-%d-%y"`.perf

	@for run in {1..$(NTESTS)}; do                    \
           source ./preface-test.sh '(metrics:time (core:load "./preface:forms-1.l" () ()) ())' >> forms-1.out; \
        done
	@mean.py forms-1.out > forms-1-`date +"%m-%d-%y"`.perf

	@for run in {1..$(NTESTS)}; do                    \
           source ./preface-test.sh '(metrics:time (core:load "./preface:forms-2.l" () ()) ())' >> forms-2.out; \
        done
	@mean.py forms-2.out > forms-2-`date +"%m-%d-%y"`.perf

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
	@echo
	@echo ';;; per test object consumption'
	@for i in nil mu core mapc forms-0 forms-1 forms-2; do	\
	    summary.py $$i $$i.out;				\
	done

base:
	@cp mu-`date +"%m-%d-%y"`.perf mu-base.perf
	@cp core-`date +"%m-%d-%y"`.perf core-base.perf
	@cp nil-`date +"%m-%d-%y"`.perf nil-base.perf
	@cp mapc-`date +"%m-%d-%y"`.perf mapc-base.perf
	@cp forms-0-`date +"%m-%d-%y"`.perf forms-0-base.perf
	@cp forms-1-`date +"%m-%d-%y"`.perf forms-1-base.perf
	@cp forms-2-`date +"%m-%d-%y"`.perf forms-2-base.perf

prof: prep
	@bash ./preface-test.sh '(metrics:time (core:load "./preface:forms-2.l" () ()) ())'
	@$(LLVMTOOLSDIR)/llvm-profdata show --all-functions --topn=25 default.profraw > profile.report 2>&1

valgrind: memcheck callgrind cachegrind

memcheck: prep
	@valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=10 \
		--leak-resolution=med --show-leak-kinds=definite \
		./eko/bin/mu-runtime                                           \
		    -l ./eko/eko/eko:core.l	       	                       \
		    -q '(core:load "./eko/eko/eko:preface.l" () ())'       \
		    -q "(core:eval (core:compile '(defconst preface::require-path \"./eko/lib-modules\")))" \
		    -q "(core:eval (core:compile '(preface:require-lib metrics)))"  \
		    -q "(core:eval (core:compile '(metrics:time (core:load \"./preface:forms-2.l\" () ()) ())))" > memcheck.report 2>&1 || true

callgrind: prep
	@valgrind --tool=callgrind 	\
		./eko/bin/mu-runtime                                           \
		    -l ./eko/eko/eko:core.l	       	                       \
		    -q '(core:load "./eko/eko/eko:preface.l" () ())'       \
		    -q "(core:eval (core:compile '(defconst preface::require-path \"./eko/lib-modules\")))" \
		    -q "(core:eval (core:compile '(preface:require-lib metrics)))"  \
		    -q "(core:eval (core:compile '(metrics:time (core:load \"./preface:forms-2.l\" () ()) ())))" > callgrind.report 2>&1 || true

	@callgrind_annotate --auto=yes callgrind.out.* >> callgrind.report
	@rm -f callgrind.out.*

cachegrind: prep
	@valgrind --tool=cachegrind	\
	./eko/bin/mu-runtime                                           	\
		    -l ./eko/eko/eko:core.l	       	                \
		    -q '(core:load "./eko/eko/eko:preface.l" () ())'    \
		    -q "(core:eval (core:compile '(defconst preface::require-path \"./eko/lib-modules\")))" \
		    -q "(core:eval (core:compile '(preface:require-lib metrics)))"  \
		    -q "(core:eval (core:compile '(metrics:time (core:load \"./preface:forms-2.l\" () ()) ())))" > cachegrind.report 2>&1 || true

clean:
	@rm -rf *-*-*-* *.out *.profraw callgrind.out.* cachegrind.out.* cachegrind.out eko