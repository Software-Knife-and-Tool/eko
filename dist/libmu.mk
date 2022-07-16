#
# libmu.a makefile
#
.PHONY: release debug profile clean libmu.a

CXX      = $(CXX)
AR       = $(AR)
CXXFLAGS = $(CXXFLAGS)
CPPFLAGS = -I ../src

OBJS =  \
    compile.o     	\
    cons.o        	\
    context.o     	\
    env.o         	\
    eval.o	       	\
    exception.o   	\
    fixnum.o      	\
    float.o       	\
    function.o    	\
    heap.o		\
    libmu.o            	\
    namespace.o   	\
    print.o       	\
    read.o        	\
    readtable.o   	\
    stream.o      	\
    symbol.o      	\
    type.o     	   	\
    vector.o		\
    heap-mapped.o      	\
    env-context.o       \
    env-env.o           \
    env-frame.o         \
    env-namespace.o     \
    mu-char.o          	\
    mu-cons.o          	\
    mu-core.o          	\
    mu-exception.o     	\
    mu-fixnum.o        	\
    mu-float.o         	\
    mu-function.o      	\
    mu-namespace.o     	\
    mu-printer.o       	\
    mu-reader.o        	\
    mu-stream.o        	\
    mu-symbol.o        	\
    mu-vector.o		\
    sys-functions.o	\
    sys-socket.o       	\
    sys-stream.o       	\
    sys-system.o

vpath %.cc ../src/libmu
vpath %.cc ../src/libmu/core
vpath %.cc ../src/libmu/env
vpath %.cc ../src/libmu/heap
vpath %.cc ../src/libmu/mu
vpath %.cc ../src/libmu/system
vpath %.cc ../src/libmu/type

libmu.a: $(OBJS)
	@$(AR) cr libmu.a $(OBJS)
	@rm -f $(OBJS)

release debug profile: libmu.a

clean:
	@rm -rf libmu.a $(OBJS)

.SILENT: $(OBJS)
