#
# top-level makefile
#
.PHONY: clean commit debug dist doc
.PHONY: install perf profile release
.PHONY: tests uninstall world
.PHONY: etags ctags valgrind prof

# assume GNU make
MAKE = make

help:
	@echo "--- world"
	@echo "    world - build release distribution"
	@echo "--- build options"
	@echo "    debug - build debug"
	@echo "    release - build release"
	@echo "    profile - build profile"
	@echo "--- distribution options"
	@echo "    doc - generate documentation"
	@echo "    dist - build distribution image"
	@echo "    install - install distribution (needs sudo)"
	@echo "    uninstall - uninstall distribution (needs sudo)"
	@echo "--- development options"
	@echo "    clean - remove non-release files"
	@echo "    commit - run linters"
	@echo "    ctags - generate ctags"
	@echo "    etags - generate etags"
	@echo "    tests - run acceptance tests"
	@echo "--- performance options"
	@echo "    perf/base - establish base"
	@echo "    perf/perf - run performance tests"
	@echo "    perf/report - test report"
	@echo "    prof - generate profiling report"
	@echo "    valgrind - generate valgrind report"

world: release

release: clean
	@$(MAKE) -C ./dist release --no-print-directory
	@$(MAKE) -C ./dist -f dist.mk --no-print-directory

debug: clean
	@$(MAKE) -C ./dist debug --no-print-directory
	@$(MAKE) -C ./dist -f dist.mk --no-print-directory

profile: clean
	@$(MAKE) -C ./dist profile --no-print-directory
	@$(MAKE) -C ./dist -f dist.mk --no-print-directory

prof:
	@$(MAKE) profile
	@$(MAKE) -C ./perf prof --no-print-directory

perf/base:
	@echo ';;; this will take a while'
	@NTESTS=50 $(MAKE) -C perf perf base

perf/perf:
	@$(MAKE) -C perf perf

perf/report:
	@$(MAKE) -C perf report

valgrind:
	@$(MAKE) debug
	@$(MAKE) -C ./perf valgrind

etags:
	@ctags -e -R src/libmu

ctags:
	@ctags -R src/libmu

dist:
	@$(MAKE) -C ./dist -f dist.mk --no-print-directory

doc:
	@$(MAKE) -C ./doc --no-print-directory

install:
	@$(MAKE) -C ./dist uninstall install -f install.mk --no-print-directory

uninstall:
	@$(MAKE) -C ./dist uninstall -f install.mk --no-print-directory

commit:
	@$(MAKE) -C ./dist tidy format cppcheck | grep -v warnings

tests:
	@$(MAKE) -C ./tests --no-print-directory

check:
distcheck:

clean:
	@rm -f TAGS tags
	@$(MAKE) -C ./dist clean --no-print-directory
	@$(MAKE) -C ./tests clean --no-print-directory
	@$(MAKE) -C ./perf clean --no-print-directory
