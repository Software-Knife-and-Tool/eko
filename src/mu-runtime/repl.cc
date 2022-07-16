/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 *(putnamjm.design@gmail.com)
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  repl.cc: mu-runtime repl
 **
 **/
#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>

#include "libmu/libmu.h"

auto load(void *context, const char *filespec) -> void {
  auto fs = std::string(filespec);

  auto load_form = "(mu:open :file :input \"" + fs + "\")";
  auto istream = libmu::api::mu_eval(
      context, libmu::api::mu_read_cstr(context, load_form.c_str()));

  if (istream == libmu::api::mu_nil()) {
    std::cerr << "unable to open " + fs + " for reading" << std::endl;
    exit(EXIT_FAILURE);
  }

  while (!libmu::api::mu_eof(context, istream))
    libmu::api::mu_eval(context, libmu::api::mu_read_stream(context, istream));
}

auto repl(void *context, int argc, char **argv) -> void {
  bool repl = argc == 1;

  libmu::api::mu_with_exception(
      context, true, [argc, argv, &repl](void *context) {
        extern char *optarg;
        extern int optind;

        int opt;
        while ((opt = getopt(argc, argv, "bvhe:q:l:")) != -1) {
          switch (opt) {
          case '?':
          case 'h': {
            const char *helpmsg =
                "OVERVIEW: mu-runtime - posix system mu interface\n"
                "USAGE: mu-runtime [options] [src-file...]\n"
                "\n"
                "OPTIONS:\n"
                "  -h                   print this message\n"
                "  -v                   print version string\n"
                "  -b                   enter break loop\n"
                "  -l SRCFILE           load SRCFILE in sequence\n"
                "  -e SEXPR             evaluate SEXPR and print result\n"
                "  -q SEXPR             evaluate SEXPR quietly\n"
                "  src-file...          load source files\n";

            std::cout << helpmsg << std::endl;
            if (opt == '?')
              return;
            break;
          }
          case 'b':
            repl = true;
            break;
          case 'l': {
            load(context, optarg);
            break;
          }
          case 'q':
            (void)libmu::api::mu_eval(
                context, libmu::api::mu_read_cstr(context, optarg));
            break;
          case 'e':
            libmu::api::mu_writeln(
                context,
                libmu::api::mu_eval(context,
                                    libmu::api::mu_read_cstr(context, optarg)),
                libmu::api::mu_nil(), false);
            break;
          case 'v':
            std::cout << libmu::api::mu_version() << std::endl;
            break;
          }
        }

        /* load files from command line */
        while (optind < argc)
          load(context, argv[optind++]);
      });

  if (repl)
    libmu::api::mu_with_exception(context, true, [](void *context) {
      for (;;) {
        if (feof(stdin))
          exit(0);

        libmu::api::mu_writeln(
            context,
            libmu::api::mu_eval(context, libmu::api::mu_read_stream(
                                             context, libmu::api::mu_t())),
            libmu::api::mu_nil(), true);
      }
    });
}

auto main(int argc, char *argv[]) -> int {
  auto ctx = libmu::api::mu_context(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO,
                                    argv, argc);

  repl(ctx, argc, argv);
  return EXIT_SUCCESS;
}
