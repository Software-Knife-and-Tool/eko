/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 ** sys-stream.cc: libmu system stream functions
 **
 **/
#include "libmu/system/stream.h"

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

namespace libmu {
namespace system {

bool Stream::WriteByte(int ch) const {
  bool rc = true;

  assert(flags & STREAM_OUTPUT);

  if ((flags & STREAM_CLOSED) == 0) {
    if (flags & STREAM_STRING) {
      sstream->str(sstream->str() + (char)ch);
    } else if (flags & STREAM_STD) {
      switch (std_stream) {
      case STDOUT:
        rc = putc(ch, stdout) == EOF;
        break;
      case STDERR:
        rc = putc(ch, stderr) == EOF;
        break;
      default:
        throw std::runtime_error("stream error");
      }
    } else if (flags & STREAM_IOS) {
      ostream->put(ch);
      rc = (ostream->rdstate() & std::ostream::badbit) == EOF;
    } else {
      throw std::runtime_error("stream error");
    }
  }

  return rc;
}

std::optional<int> Stream::ReadByte() const {
  int ch;

  assert(flags & STREAM_INPUT || flags & STREAM_STRING);

  if (flags & STREAM_CLOSED)
    return std::nullopt;

  if (flags & STREAM_STRING) {
    ch = sstream->get();
    if (sstream->eof())
      ch = EOF;
  } else if (flags & STREAM_STD) {
    switch (std_stream) {
    case STDIN:
      ch = getc(stdin);
      break;
    default:
      throw std::runtime_error("read stream");
    }
  } else if (flags & STREAM_IOS) {
    ch = istream->get();
    if (istream->eof())
      ch = EOF;
  } else {
    throw std::runtime_error("read stream");
  }

  if (ch == 0x4 || ch == EOF)
    return std::nullopt;

  return ch;
}

std::optional<int> Stream::UnReadByte(int ch) const {
  int rc = ch;

  assert(flags & STREAM_INPUT || flags & STREAM_STRING);

  if (flags & STREAM_CLOSED)
    return std::nullopt;

  if (flags & STREAM_STRING) {
    sstream->putback(ch);
  } else if (flags & STREAM_STD) {
    switch (std_stream) {
    case STDIN:
      rc = ungetc(ch, stdin);
      break;
    default:
      throw std::runtime_error("unread error");
    }
  } else if (flags & STREAM_IOS) {
    istream->putback(ch);
    rc = (istream->rdstate() & std::istream::badbit) ? EOF : 0;
  } else {
    throw std::runtime_error("stream type violation");
  }

  if (rc == EOF)
    return std::nullopt;

  return rc;
}

std::optional<Stream *> Stream::OpenInputFile(const std::string &pathname) {
  struct stat fileInfo;

  if (stat(pathname.c_str(), &fileInfo) == -1)
    return std::nullopt;

  auto ifs = new std::ifstream();

  ifs->open(pathname, std::fstream::in);
  if (ifs->fail())
    return std::nullopt;

  return new Stream(ifs, STREAM_IOS | STREAM_INPUT);
}

std::optional<Stream *> Stream::OpenOutputFile(const std::string &pathname) {
  auto ofs = new std::ofstream;

  ofs->open(pathname, std::fstream::out);
  if (ofs->fail())
    return std::nullopt;

  return new Stream(ofs, STREAM_IOS | STREAM_OUTPUT);
}

std::optional<Stream *> Stream::OpenOutputString(const std::string &init) {
  auto oss = new std::stringstream(init);

  if (oss->fail())
    return std::nullopt;

  return new Stream(oss, STREAM_STRING | STREAM_OUTPUT);
}

std::optional<Stream *> Stream::OpenInputString(const std::string &str) {
  auto ifs = new std::stringstream(str);

  if (ifs->fail())
    return std::nullopt;

  return new Stream(ifs, STREAM_STRING | STREAM_INPUT);
}

Stream *Stream::OpenStandardStream(STD_STREAM stream, int fd) {
  switch (stream) {
  case STD_STREAM::STDIN:
    return new Stream(stream, fd, STREAM_STD | STREAM_INPUT);
  case STD_STREAM::STDOUT:
    [[fallthrough]];
  case STD_STREAM::STDERR:
    return new Stream(stream, fd, STREAM_STD | STREAM_OUTPUT);
  default:
    throw std::runtime_error("stream type violation");
  }
}

void Stream::Close() {
  if (flags & STREAM_STD)
    return;

  if (flags & STREAM_OUTPUT)
    ostream->flush();

  flags |= STREAM_CLOSED;
}

} /* namespace system */
} // namespace libmu
