## *ekō* - A Lighter Shade of LISP 

_eko_[1]: a single process LISP system development and runtime platform for low overhead environments.

#### Rationale

------

Dynamic languages for low overhead environments need to be lightweight, but developing applications in the target environment can be painful. *eko* is a general development and execution environment for containers, shells, and other utilities that need a small installation and runtime footprint.

There are a number of big LISPs that require a fair amount of system resources to perform well. *SBCL*, *Clojure* and the like are excellent application development platforms.

*eko* has been pared down to operate effectively on Raspberry Pi class machines and containers. Likewise, there are a number of well-written smaller Scheme-like LISPs that target very low resource environments like Arduinos. *eko*, though similar to a LISP-1 Scheme in many respects, attempts to appeal to the more traditional LISP programmer who prefers the Common LISP macro system.

*eko* consists of a small native-code kernel runtime and a set of layered libraries in source form.

The *eko* runtime is a native code library that directly interprets *mu* kernel code. A great deal of useful work can be done directly with the limited kernel language, though it lacks the niceties of a more canonical LISP programming environment. More advanced language features require the use of the included *core* library. The *preface* library provides traditional LISP forms and a collection of functional programming features like pattern matching.

*eko* core and library code are human-readable and portable between machine architectures.

*eko* provides a more or less traditional REPL in which applications can be developed and largely verified in a host environment and then packaged and deployed to a target environment as a heap image.



#### Goals

------

- minimal POSIX LISP runtime suitable for containers
- native code compiler
- small and simple installation, self-contained
- add interactivity and extensibility to application implementations
- effective FFI system
- resource overhead equivalent to a UNIX shell



#### State of the *eko* system

------

*eko* is a work in progress, and is evolving from a desktop application development environment to one more suitable for limited environments. 

*eko* builds with *gcc-12* and runs on:

- x86-64 and AArch-64 Linux distributions
- x86-64 and M1 MacOs X
- x86-64 WSL

Clang 13+ should also build *eko* correctly.

Portability, libraries, deployment, documentation, and garbage collection are currently the top priorities.

See the documentation for an up to date list of what works and what doesn't. The reference cards document currently available symbols.



#### About *eko*

------

*eko* is an immutable, namespaced *LISP-1* that borrows heavily from *Scheme*, but is stylistically more closely related to the Common LISP family of languages. *eko* syntax and constructs will be familiar to the traditional LISP programmer.

The _eko_ runtime kernel is written in C++2b, compiles with recent *gcc* or *clang* compilers, and requires only the standard library.

The _libmu_ runtime implements 64 bit tagged pointers and can accommodate an address space up to 59 bits. _libmu_ is available as a library, extends a C/C++ API for embedded applications, and is an evaluator for the kernel language _mu_.  _mu_ provides the usual fixed-width numeric types, lists, fixed-arity lambdas, exceptions, symbol namespaces, streams, and specialized vectors in a garbage collected environment.

The *core* library provides *rest* lambdas, closures, defun/defconst/defmacro and a compiler/evaluator for those forms.

*preface* extends *core* with various lexical binding forms, *cond/and/or/progn*, and a library loading facility.

Optional libraries provide a variety of enhancements and services, including Common LISP macros and binding special forms.



#### Viewing the documentation

------

The _eko_ documentation is a collection of *markdown* files in `doc/reference`, currently not complete. The *html* version of those files is available in the `doc/html` directories in both the source and release directories.

To browse the documentation, start with `eko.html`. 



#### Building and installing the _eko_ system

------

The _eko_ runtime _libmu_ is a native code program that must be built for the target CPU architecture. The _eko_ build system requires only a `c++` compiler and some form of the `make` utility. Other tools like `cppcheck`, `clang-tidy`, and `valgrind` are optional. Performance measurement requires some version of `python 3`.

After cloning the _eko_ repository, the _eko_ system can be built and installed with the supplied makefile. There is no `./configure` step as is common in similar projects.

```
% git clone https://github.com/Software-Knife-and-Tool/eko.git
% cd eko
% make world
% sudo make install
```

This builds and installs the _eko_ system in `/opt/eko`.  `make` with no arguments prints the available targets, with `world` builds the `release` target that compiles the runtime and creates an install image. Related build targets, `debug` and `profile`, are used to compile for debugging and profiling respectively.

If you want to just repackage *eko* after a change to the library sources:

```
% make dist
```

and then install.



#### Testing and performance metrics

------

The distribution includes a test suite, which should be run after every interesting change and prior to any check in. The test suite consists of a couple of hundred or so individual tests separated into multiple sections, roughly separated by namespace.

Failures in the early tests are almost guaranteed to cause complete failure of subsequent tests.

```
% make test
```

Performance metrics, which can take in excess of ten minutes to run, can be captured with

```
% make -C perf perf
```

Metrics include the average amount of time taken for an individual test and the number of objects allocated by that test. Differences between runs in the same installation can be in the 1% - 3% range. If you want an extended test, the NTESTS environment variable controls how many runs are included in a single test run. The default NTESTS is 10.

```
% make -C perf base
```

establishes a new baseline from the current metrics. Typically, you'd make a perf run and establish a baseline, make a change, and run the perf tests again to see how your changes affected the performance tests. Test results are not checked into the project. Once you've done that, subsequent perf runs will show the difference between the established baseline and the current run.

```
% make -C perf report
```

produces a synopsis of the difference between the current binaries and sources and the established baseline along with other interesting statistics.

For convenience, the *eko* Makefile provides

```
% make perf/base      # establish an NTESTS=50 baseline, will take some time to run
% make perf/perf      # run a shorter perf test, NTESTS=10
% make perf/report	  # compare baseline and most recent perf run
```



#### Profiling and valgrind reports

-----

If you want to profile the runtime and have installed the needed instrumentation (llvm-profdata and valgrind) you can generate those reports with

```
% make prof
% make valgrind
```

The *eko* Makefile will compile the appropriate runtime and test it. You may need to rebuild the desired distribution afterward for further use.



#### Building on *MacOS*

------

All the tools necessary to build the *eko* system on *MacOS* are installed with *Xcode*.

The default compiler is `c++`.  The `eko/release/Makefile` `CXX`  variable specifies the path to your chosen compiler driver. On *MacOS* the default compiler is `clang++`, which is installed as part of *Xcode* and is aliased to `c++`. 

If you want to use the *clang* and related tools, you can install them with the *Homebrew* package manager (see https://brew.sh/ for details) and adjust `eko/release/Makefile` accordingly.



#### Building on *Linux*

------

All the tools necessary to build the *eko* system on *Linux* are installed with the *build-essential* package for Debian-based systems with the `apt` package manager. Windows WSL/WSL2 systems should be similar.

If not already installed, the `build-essential` package contains the GNU `g++` compiler and `make` utility.

```
% sudo apt-get install build-essential
```



------

Other Linux systems are of interest but currently unexamined.

The default compiler is `c++`.   The ` eko/release/Makefile`  `CXX `  rule specifices the path to your chosen compiler driver. On *Linux* the default compiler is `g++`, which is installed as part of the *build-essentials* package and is aliased to `c++`. 

This all seems to work for Debian distributions on x86-64. For ARM, as on Raspberry Pis, you may need to separately install g++ and amend `dist/Makefile` accordingly. You'll need to run a 64 bit OS and compile with `-march=armv8-a`.



#### Running the _eko_ system

------

The _eko_ binaries, libraries, and source files are installed in `/opt/eko` . The `bin` directory contains the binaries and shell scripts  for running the system.

```
mu-runtime    runtime binary, minimal repl
eko           shell script for running the extended repl
```

``` 
OVERVIEW: mu-runtime - posix platform mu interface
USAGE: mu-runtime [options] [src-file...]

OPTIONS:
  -h                   print this message
  -v                   print version string
  -b                   enter break loop
  -l SRCFILE           load SRCFILE in sequence
  -e SEXPR             evaluate SEXPR and print result
  -q SEXPR             evaluate SEXPR quietly
  src-file ...         load source files
  
```

An interactive session for the extended _eko_ system is invoked by `eko`, `:h` will print the currently available repl commands. Forms entered at the prompt are evaluated and the results printed. The prompt displays the current namespace.

```
% /opt/eko/bin/eko
;;;
;;; Eko Lisp 0.0.1
;;; (preface:repl) :h for help
;;;
user> 
```



------

1. What is the color of an echo? 

   LISP is a path, one of many. Unlike many languages, LISPs are intentionally dynamic which has selected against them for use in production environments, yet statically-typed languages produce systems that are hard to change and even harder to interact with.
   
   Change and evolution are the only defenses a system has against obsolescence.  
   
   Dynamic systems are the next step.
   
   ekō エコー
   
   
