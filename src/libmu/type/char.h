/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  char.h: library chars
 **
 **/
#if !defined(LIBMU_TYPE_CHAR_H_)
#define LIBMU_TYPE_CHAR_H_

#include <cassert>
#include <map>

#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/fixnum.h"
#include "libmu/type/stream.h"

namespace libmu {

using Tag = core::Type::Tag;
using DIRECT_CLASS = core::Type::DIRECT_CLASS;

namespace type {

class Char : public Type {
public:
  static inline bool IsType(Tag ptr) {
    return ((TagOf(ptr) == TAG::DIRECT) &&
            (DirectClass(ptr) == DIRECT_CLASS::CHAR) && (DirectSize(ptr) > 0));
  }

  static uint64_t UInt64Of(Tag ptr) {
    assert(IsType(ptr));

    return DirectData(ptr);
  }

  static void Write(Env &env, Tag ch, Tag stream, bool esc) {
    assert(IsType(ch));
    assert(Stream::IsType(env, stream));

    if (esc)
      Env::Write(env, "#\\", stream, false);

    Stream::WriteChar(env, ch, stream);
  }

  Tag Heap(Env &) override { return tag_; };

  /** * a char is a char immediate with a nonzero length **/
  explicit Char(uint8_t ch) : Type() {

    tag_ = MakeDirect(ch, 1, DIRECT_CLASS::CHAR);
  }

public: /* object */
  Tag tag_;

  explicit Char(Tag fx) : Type() {
    assert(Fixnum::IsType(fx));

    tag_ = MakeDirect(Fixnum::Int64Of(fx), 1, DIRECT_CLASS::CHAR);
  }

}; /* class Char */

} /* namespace type */
} /* namespace libmu */

#endif /* LIBMU_TYPE_CHAR_H_ */
