#
# libmu.so makefile
#
CXX      = $(CXX)
AR       = $(AR)
CXXFLAGS = $(CXXFLAGS)
CPPFLAGS = -I ../src

.PHONY: clean libmu.so

libmu.so: libmu.a
	@$(AR) x libmu.a
	@$(CXX) -shared $(CXXFLAGS) *.o -o libmu.so
	@rm -f *.o *__.SYMDEF*

clean:
	@rm -rf libmu.so *.o *__SYMDEF*
