#
# tests makefile
#
.PHONY: tests clean

tests: ../dist/mu-runtime
	@./run-mu-tests mu
	@./run-mu-tests env
	@./run-mu-tests system
	@./run-mu-tests core "-l ../dist/eko:core.l"
	@./run-mu-tests core:compile "-l ../dist/eko:core.l"
	@./run-core-tests ./core.l
	@./run-preface-tests ./preface:forms-0.l
	@./run-preface-tests ./preface:forms-1.l
	@./run-preface-tests ./preface:forms-2.l
	@./run-preface-tests ./preface:forms-3.l

../dist/mu-runtime:
	@echo "make release to create mu-runtime first"
	exit 1

clean:
	@rm -f /var/tmp/test.* /var/tmp/result.* /var/tmp/expect.*	

