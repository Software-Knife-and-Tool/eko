#
# posix install makefile
#
ROOT = /opt
BASE = eko

.PHONY: install uninstall help

help:
	@echo install - install ix in $(ROOT)/$(BASE) (needs sudo)
	@echo uninstall - remove ix from $(ROOT) (needs sudo)

install:
	@cat ./$(BASE)-0.0.1.tgz | (cd $(ROOT); tar xfz -)

uninstall:
	@rm -rf $(ROOT)/$(BASE)
