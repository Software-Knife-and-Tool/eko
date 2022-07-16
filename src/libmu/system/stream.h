/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  system.h: system abstraction
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

#include "libmu/system/socket.h"

#if !defined(LIBMU_SYSTEM_STREAM_H_)
#define LIBMU_SYSTEM_STREAM_H_

namespace libmu {
namespace system {

struct Stream {
public:
  enum STD_STREAM { STDIN, STDOUT, STDERR };

  static const int STREAM_INPUT = 0x0001;
  static const int STREAM_OUTPUT = 0x0002;
  static const int STREAM_IOS = 0x0004;
  static const int STREAM_STRING = 0x0008;
  static const int STREAM_FDESC = 0x0010;
  static const int STREAM_STD = 0x0020;
  static const int STREAM_SOCKET = 0x0040;
  static const int STREAM_CLOSED = 0x0080;

private:
  [[maybe_unused]] int sostream;
  int std_stream;
  std::istream *istream;
  std::ostream *ostream;
  std::stringstream *sstream;

  int flags;

public:
  bool IsClosed() const { return flags & Stream::STREAM_CLOSED; }
  bool IsInput() const { return flags & STREAM_INPUT; }
  bool IsOutput() const { return flags & STREAM_OUTPUT; }
  bool IsString() const { return flags & STREAM_STRING; }

  bool IsEof() const {
    assert(IsInput());

    if (IsString())
      return sstream->eof();

    return flags & STREAM_STD ? feof(stdin) : istream->eof();
  }

  std::string GetStdString() const {
    assert(IsString() && IsOutput());

    std::string str = sstream->str();

    sstream->str(std::string());
    return str;
  }

  void Flush() const {
    if ((flags & STREAM_STD) && (flags & STREAM_OUTPUT))
      fflush(stdout);
  }

  void Close();

  std::optional<int> ReadByte() const;
  std::optional<int> UnReadByte(int) const;
  bool WriteByte(int) const;

  static std::optional<Stream *> OpenInputFile(const std::string &);
  static std::optional<Stream *> OpenInputString(const std::string &);
  static std::optional<Stream *> OpenOutputFile(const std::string &);
  static std::optional<Stream *> OpenOutputString(const std::string &);

  static Stream *OpenStandardStream(STD_STREAM, int fd);

  Stream(std::istream *is, int flags) : istream(is), flags(flags) {}
  Stream(std::ostream *os, int flags) : ostream(os), flags(flags) {}
  Stream(std::stringstream *ss, int flags) : sstream(ss), flags(flags) {}
  // NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
  Stream(int fd, int flags) : sostream(fd), flags(flags) {}
  // NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
  Stream(STD_STREAM, int fd, int flags) : std_stream(fd), flags(flags) {}
};

} /* namespace system */
} /* namespace libmu */

#endif /* LIBMU_SYSTEM_STREAM_H_ */
