#
# posix platform makefile
#
MAKE        = make
AR          = ar

# for x64_86 g++
CXX         = g++-12
DCXXFLAGS   = -m64 -Wall -Wpedantic -Wextra -std=c++2b -O3 -g -fPIC -pthread
PCXXFLAGS   = -m64 -Wall -Wpedantic -std=c++2b -fprofile-instr-generate -fcoverage-mapping -fPIC -pthread
RCXXFLAGS   = -m64 -Wall -Wpedantic -Wextra -std=c++2b -O3 -fPIC -DNDEBUG -pthread

# for aarch64 g++ on Raspberry Pi 3/4
# CXX         = g++12
# DCXXFLAGS   = -march=armv8-a -Wall -Wpedantic -Wextra -std=c++2b -g -fPIC -pthread
# PCXXFLAGS   = -march=armv8-a -Wall -Wpedantic -std=c++2b -fprofile-instr-generate -fcoverage-mapping -fPIC -pthread
# RCXXFLAGS   = -march=armv8-a -Wall -Wpedantic -Wextra -std=c++2b -O3 -fPIC -pthread

DMAKEFLAGS = CXX="$(CXX)" AR="$(AR)" CXXFLAGS="$(DCXXFLAGS)"
PMAKEFLAGS = CXX="$(CXX)" AR="$(AR)" CXXFLAGS="$(PCXXFLAGS)"
RMAKEFLAGS = CXX="$(CXX)" AR="$(AR)" CXXFLAGS="$(RCXXFLAGS)"

MAKES = libmu.mk libmu.so.mk mu-runtime.mk

.PHONY:  format release debug clean tidy cppcheck

SRCDIRS =	\
	../src/libmu		\
	../src/libmu/core	\
	../src/libmu/heap	\
	../src/libmu/mu		\
	../src/libmu/types	\
	../src/libmu/platform	\
        ../src/mu-runtime	

all:
	@echo make clean - clean executables and libraries
	@echo make debug - build for debugging
	@echo make profile - build for profiling
	@echo make release - build for release

release:
	@for i in $(MAKES); do		\
            make -f $$i $(RMAKEFLAGS);	\
	done
	@make -f eko.mk

debug:
	@for i in $(MAKES); do		\
	    make -f $$i $(DMAKEFLAGS);	\
	done
	@make -f eko.mk

profile:
	@for i in $(MAKES); do		\
	    make -f $$i $(PMAKEFLAGS);	\
	done
	@make -f eko.mk

cppcheck:
	@cppcheck --quiet --enable=all ../src/libmu

tidy:
	@for f in $(SRCDIRS); do	\
	    clang-tidy 									\
	        -checks='cppcoreguidelines*,clang-diagnostic-*,clang-analyzer-*,-*,bugprone*,modernize*,-modernize-use-trailing-return-type, performance*,-modernize-pass-by-value,-modernize-use-auto,-modernize-use-using'	\
                -header-filter=$$f							\
		$$f/*.cc --	 							\
		-I../src;								\
	done 2>&1 | sort | uniq | grep warning

lint:
	@for f in $(SRCDIRS); do	\
	    clang-tidy 									\
	        -checks='cppcoreguidelines*,clang-diagnostic-*,clang-analyzer-*,-*,bugprone*,modernize*,-modernize-use-trailing-return-type, performance*,-modernize-pass-by-value,-modernize-use-auto,-modernize-use-using'	\
                -header-filter=$$f							\
		$$f/*.cc --	 							\
		-I../src;								\
	done 2>&1 | sort | uniq | grep error

format:
	@(cd ../src/libmu && clang-format -i *.h *.cc */*.h */*.cc)
	@(cd ../src/mu-runtime && clang-format -i *.cc)

clean:
	@rm -f __.SYMDEF
	@for i in $(MAKES); do		\
	    $(MAKE) -f $$i clean;	\
	done
	@make -f dist.mk clean
	@make -f eko.mk clean
