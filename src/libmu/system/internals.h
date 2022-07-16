/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  internals.h: system functions
 **
 **/
#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "libmu/system/stream.h"

#if !defined(LIBMU_SYSTEM_INTERNALS_H_)
#define LIBMU_SYSTEM_INTERNALS_H_

namespace libmu {
namespace system {

struct System {
public:
  typedef std::pair<const std::string, const std::string> OptMap;
  typedef std::list<OptMap>::iterator OptMapPtr;

  std::unique_ptr<std::list<OptMap>> options;
  std::unique_ptr<std::list<std::string>> optargs;
  OptMapPtr next;

  std::string map_name;
  char *base;

  Stream *stdin;
  Stream *stdout;
  Stream *stderr;

public: /* options */
  void PushArg(const char *arg) { optargs->push_back(arg); }

  void PushOpt(int key, const char *value) {
    std::string cstr = std::string(1, static_cast<char>(key));

    options->push_back(std::make_pair(cstr, value == nullptr ? "" : value));
  }

  OptMapPtr Find(const std::string &key) {
    next = options->begin();
    return std::find_if(
        options->begin(), options->end(),
        [key](const OptMap &el) { return el.first.compare(key) == 0; });
  }

  OptMapPtr FindNext(const std::string &key) {
    return std::find_if(next, options->end(), [key](const OptMap &el) {
      return el.first.compare(key) == 0;
    });
  }

  bool IsFound(OptMapPtr el) { return el != options->end(); }

public: /* accessors */
  // const auto [name, ns] = ns_iter;
  static std::string name(OptMapPtr el) { return el->first; }
  static std::string value(OptMapPtr el) { return el->second; }

  static std::string name(const OptMap &el) { return el.first; }
  static std::string value(const OptMap &el) { return el.second; }

public: /* image mmap */
  static const int PAGESIZE = 4096;
  static std::optional<const char *> MapPages(unsigned, const char *);
  static bool SaveImage(System &, std::string);

public:
  [[noreturn]] static void Exit(int64_t);

  static int Errno();
  static std::optional<const std::string> Chdir(const std::string);
  static std::optional<const std::string> Cwd();
  static std::optional<const std::string> Logname();
  static std::vector<std::pair<const std::string, const std::string>>
  ProcessEnv();

  static std::optional<uint64_t> ProcessTime();
  static std::optional<uint64_t> SystemTime();
  static std::optional<int> SysExec(const std::string &);

public:
  explicit System(int stdin, int stdout, int stderr)
      : options(std::make_unique<std::list<OptMap>>()),
        optargs(std::make_unique<std::list<std::string>>()),
        next(options->begin()),
        stdin(Stream::OpenStandardStream(Stream::STD_STREAM::STDIN, stdin)),
        stdout(Stream::OpenStandardStream(Stream::STD_STREAM::STDOUT, stdout)),
        stderr(
            Stream::OpenStandardStream(Stream::STD_STREAM::STDERR, stderr)){};
};

} /* namespace system */
} /* namespace libmu */

#endif /* LIBMU_SYSTEM_INTERNALS_H_ */
