/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  stream.h: library streams
 **
 **/
#if !defined(LIBMU_TYPE_STREAM_H_)
#define LIBMU_TYPE_STREAM_H_

#include <cassert>
#include <memory>
#include <string>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/system/stream.h"

namespace libmu {

using Type = core::Type;
using Tag = core::Type::Tag;
using Env = core::Env;
using Heap = core::Heap;

namespace type {

/** * stream type class **/
class Stream : public Type {
private:
  typedef struct {
    system::Stream *stream;
  } Layout;

  Layout stream_;

public: /* tag */
  static bool IsType(Env &, Tag);

  static system::Stream *stream(Env &env, Tag stream) {
    assert(IsType(env, stream));

    return Heap::Layout<Layout>(env, stream)->stream;
  }

  static Tag OpenInputFile(Env &env, const std::string &path) {
    auto stream = system::Stream::OpenInputFile(path);

    if (!stream.has_value())
      return NIL;

    return Stream(stream.value()).Heap(env);
  }

  static Tag OpenOutputFile(Env &env, const std::string &path) {
    auto stream = system::Stream::OpenOutputFile(path);

    if (!stream.has_value())
      return NIL;

    return Stream(stream.value()).Heap(env);
  }

  static Tag OpenInputString(Env &env, const std::string &str) {
    auto stream = system::Stream::OpenInputString(str);

    if (!stream.has_value())
      return NIL;

    return Stream(stream.value()).Heap(env);
  }

  static Tag OpenOutputString(Env &env, const std::string &str) {
    auto stream = system::Stream::OpenOutputString(str);

    if (!stream.has_value())
      return NIL;

    return Stream(stream.value()).Heap(env);
  }

  static Tag Close(Env &, Tag);

  static Tag ReadByte(Env &, Tag);
  static Tag UnReadByte(Env &, Tag, Tag);

  static void WriteByte(Env &, Tag, Tag);

  static Tag ReadChar(Env &, Tag);
  static Tag UnReadChar(Env &, Tag, Tag);
  static void WriteChar(Env &, Tag, Tag);

  static bool IsClosed(Env &, Tag);
  static bool IsEof(Env &, Tag);
  static void Flush(Env &, Tag);

  static void GcMark(Env &, Tag);
  static Tag View(Env &, Tag);

public: /* type model */
  Tag Heap(Env &) override;

public: /* object */
  explicit Stream() = default;
  explicit Stream(system::Stream *);
  explicit Stream(Env &, system::Stream *);

  ~Stream() override = default;
}; /* class Stream */

} /* namespace type */
} /* namespace libmu */

#endif /* LIBMU_TYPE_STREAM_H_ */
