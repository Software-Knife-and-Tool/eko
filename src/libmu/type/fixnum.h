/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  fixnum.h: library fixnums
 **
 **/
#if !defined(LIBMU_TYPE_FIXNUM_H_)
#define LIBMU_TYPE_FIXNUM_H_

#include <cassert>

#include "libmu/core/type.h"

namespace libmu {

using Env = core::Env;
using Tag = core::Type::Tag;
using Type = core::Type;

namespace type {

/** * fixnum type class **/
class Fixnum : public Type {
public: /* tag */
  static inline bool IsType(Tag ptr) {
    return IsDirect(ptr) && DirectClass(ptr) == DIRECT_CLASS::FIXNUM;
  }

  static bool IsByte(Tag ptr) { return IsType(ptr) && Int64Of(ptr) < 256; }

  static inline int64_t Int64Of(Tag fx) {
    assert(IsType(fx));

    return static_cast<int64_t>(std::to_underlying(fx)) >> 5;
  }

  static int64_t VSpecOf(Tag fx) { return Int64Of(fx); }
  static void Write(Env &, Tag, Tag, bool);

public: /* type model */
  Tag tag_;

  Tag Heap(Env &) override { return tag_; }

public: /* object */
  explicit Fixnum(int64_t fx) : Type() {
    tag_ = Tag((fx << 5) | (std::to_underlying(DIRECT_CLASS::FIXNUM) << 2) |
               std::to_underlying(TAG::DIRECT));
  }

  explicit Fixnum(size_t fx) : Type() {
    tag_ = Tag((fx << 5) | (std::to_underlying(DIRECT_CLASS::FIXNUM) << 2) |
               std::to_underlying(TAG::DIRECT));
  }

  explicit Fixnum(int fx) : Type() {
    tag_ = Tag((fx << 5) | (std::to_underlying(DIRECT_CLASS::FIXNUM) << 2) |
               std::to_underlying(TAG::DIRECT));
  }

  explicit Fixnum(unsigned int fx) : Type() {
    tag_ = Tag((fx << 5) | (std::to_underlying(DIRECT_CLASS::FIXNUM) << 2) |
               std::to_underlying(TAG::DIRECT));
  }

  ~Fixnum() override = default;
};

} /* namespace type */
} /* namespace libmu */

#endif /* LIBMU_TYPE_FIXNUM_H_ */
