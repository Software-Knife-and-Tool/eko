/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 ** system.cc: libmu system functions
 **
 **/
#include <cassert>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <optional>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include "libmu/system/internals.h"
#include "libmu/system/stream.h"

extern char **environ;

namespace libmu {
namespace system {

/** * map pages in system address space **/
std::optional<const char *> System::MapPages(unsigned npages,
                                             const char *heapId) {
  char *base;
  int fd, seek;
  char tmpfn[PATH_MAX];

  strcpy(tmpfn, "/var/tmp/lambda-");
  strcat(tmpfn, heapId);
  strcat(tmpfn, "-XXXXXX");

  fd = mkstemp(tmpfn);
  if (fd < 0)
    return std::nullopt;

  seek = ftruncate(fd, npages * PAGESIZE);
  if (seek < 0)
    return std::nullopt;

  base = (char *)mmap(nullptr, npages * PAGESIZE, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE, fd, 0);

  if (base == (char *)-1)
    return std::nullopt;

  close(fd);
  unlink(tmpfn);

  return base;
}

/** * get system clock time in microseconds **/
std::optional<uint64_t> System::SystemTime() {
  struct timeval now;

  if (gettimeofday(&now, nullptr) < 0)
    return std::nullopt;

  return (now.tv_sec * 1e6) + now.tv_usec;
}

/** * get process elapsed time in microseconds **/
std::optional<uint64_t> System::ProcessTime() {
  struct timespec now;

  /* CLOCK_PROCESS_CPUTIME_ID may not be portable */
  if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now) < 0)
    return std::nullopt;

  return (now.tv_sec * 1e6) + (now.tv_nsec / 1e3L);
}

/** * system **/
std::optional<int> System::SysExec(const std::string &cmd) {
  int rc = ::system(cmd.c_str());

  if (rc < 0)
    return std::nullopt;

  return rc;
}

/** * exit **/
void System::Exit(int64_t rc) { ::exit(rc); }

/** * logname **/
std::optional<const std::string> System::Logname() {
  char *logname = ::getenv("LOGNAME");

  if (logname == NULL)
    return std::nullopt;

  return std::string(logname);
}

/** * system environment **/
std::vector<std::pair<const std::string, const std::string>>
System::ProcessEnv() {
  auto env_vector =
      std::vector<std::pair<const std::string, const std::string>>{};

  char **lp = environ;
  while (*lp != NULL) {
    std::string env(*lp);
    size_t sep = env.find('=');

    if (sep > env.size() || sep == 0)
      throw std::runtime_error("process environment corrupt");

    const std::string name = env.substr(0, sep);
    const std::string value = env.substr(sep + 1);

    auto pair = std::pair<const std::string, const std::string>(name, value);
    env_vector.push_back(pair);
    lp++;
  }

  return env_vector;
}

/** * get cwd **/
std::optional<const std::string> System::Cwd() {
  char path[PATH_MAX];

  if (::getcwd(path, PATH_MAX) == NULL)
    return std::nullopt;

  return std::string(path, strlen(path));
}

/** * chdir **/
std::optional<const std::string> System::Chdir(const std::string path) {
  if (::chdir(const_cast<const char *>(path.c_str())) < 0)
    return std::nullopt;

  return path;
}

/** * errno **/
bool System::SaveImage(System &, std::string) { return true; }

/** * errno **/
int System::Errno() { return errno; }

} /* namespace system */
} // namespace libmu
