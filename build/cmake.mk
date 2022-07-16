#
#
#
.PHONY: all clean

all: clean
	@cmake -D CMAKE_CXX_COMPILER=g++-12 .

clean:
	@rm -rf CMakeCache.txt CMakeFiles Makefile *.cmake
