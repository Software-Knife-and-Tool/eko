#
# posix dist makefile
#
BASE = eko
DOCS = ../doc
SRC  = ../src
DIST = ../dist

.PHONY: dist clean

dist:
	@make -f $(BASE).mk

	@install -d $(BASE)
	@install -d $(BASE)/bin
	@install -d $(BASE)/$(BASE)
	@install -d $(BASE)/lib
	@install -d $(BASE)/lib-modules
	@install -d $(BASE)/doc
	@install -d $(BASE)/doc/html
	@install -m 644 $(DOCS)/refcards/*.pdf $(BASE)/doc
	@install -m 644 $(DOCS)/html/*.html $(BASE)/doc/html
	@install -m 644 $(DIST)/eko.l $(BASE)/$(BASE)
	@install -m 644 $(DIST)/eko:preface.l $(BASE)/$(BASE)
	@install -m 644 $(DIST)/eko:core.l $(BASE)/$(BASE)
	@install -m 644 $(DIST)/eko:common.l $(BASE)/lib-modules
	@install -m 644 $(DIST)/eko:compile.l $(BASE)/lib-modules
	@install -m 644 $(DIST)/eko:metrics.l $(BASE)/lib-modules
	@install -m 644 $(DIST)/eko:system.l $(BASE)/lib-modules
	@install -m 644 $(DIST)/libmu.a $(BASE)/lib
	@install -m 644 $(DIST)/libmu.so $(BASE)/lib
	@install -m 755 $(DIST)/mu-runtime $(BASE)/bin

	@install -m 755 ./eko.sh $(BASE)/bin/eko

	@tar cfz $(BASE)-0.0.1.tgz $(BASE)
	@rm -rf $(BASE)

clean:
	@rm -rf $(BASE)-0.0.1.tgz $(BASE)
