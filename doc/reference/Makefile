#
# docs makefile
#
.PHONY: reference

SRCDIRS = common core mu eko preface system

reference:
	@for dir in $(SRCDIRS); do		\
	    for md in $$dir/*.md; do		\
	    	pandoc -s -H reference.css $$md -f markdown -o ../html/`basename $$md .md`.html;	\
	    done 				\
	done
