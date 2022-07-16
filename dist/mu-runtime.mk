#
# mu-runtime makefile
#
.PHONY: mu-runtime clean

CXX      = $(CXX)
AR       = $(AR)
CXXFLAGS = $(CXXFLAGS)
CPPFLAGS = -I ../src

OBJS = repl.o

vpath %.cc ../src/mu-runtime

LIBS = ./libmu.a	

mu-runtime: $(OBJS)
	@$(CXX) -o mu-runtime $(CPPFLAGS) $(CXXFLAGS) $(OBJS) $(LIBS)
	@rm $(OBJS)

clean:
	@rm -f $(OBJS) mu-runtime

.SILENT: $(OBJS)
