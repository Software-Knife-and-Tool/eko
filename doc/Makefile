#
# docs makefile
#
.PHONY: clean tags reference symbols

reference:
	@make -C reference

symbols:
	@bash ./symbols.sh

tags:
	ctags -R ../src/libmu
	@./tags-xref.py
