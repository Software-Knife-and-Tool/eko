/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  float.h: library floats
 **
 **/
#if !defined(LIBMU_TYPE_FLOAT_H_)
#define LIBMU_TYPE_FLOAT_H_

#include <cassert>
#include <cstring>

#include "libmu/core/env.h"
#include "libmu/core/type.h"

namespace libmu {

using Env = core::Env;
using Type = core::Type;
using Tag = Type::Tag;
using Heap = core::Heap;

namespace type {

/** * float type class **/
class Float : public Type {
public: /* tag */
  static inline bool IsType(Tag ptr) {
    return IsDirect(ptr) && (DirectClass(ptr) == DIRECT_CLASS::FLOAT);
  }

  static float FloatOf(Tag fl) {
    assert(IsType(fl));

    float buf = 0.0;

    std::memcpy(reinterpret_cast<char *>(&buf),
                reinterpret_cast<char *>(&fl) + 4, sizeof(float));
    return buf;
  }

  static float VSpecOf(Tag fl) { return FloatOf(fl); }

  static void Write(Env &, Tag, Tag, bool);

public: /* type model */
  Tag Heap(Env &) override { return tag_; }

public: /* object */
  Tag tag_;

  explicit Float(float fl) : Type() {
    uint64_t buf = 0;

    std::memcpy(reinterpret_cast<char *>(&buf) + 3,
                reinterpret_cast<char *>(&fl), sizeof(float));
    tag_ = MakeDirect(buf, 0, DIRECT_CLASS::FLOAT);
  }

  ~Float() override = default;
};

/** * double type class **/
class Double : public Type {
public:
  struct Layout {
    double dbl;
  };

  Layout layout_;

public: /* tag */
  static inline bool IsType(Env &env, Tag ptr) {
    return IsIndirect(ptr) && env.heap->SysClass(env, ptr) == SYS_CLASS::DOUBLE;
  }

  static double DoubleOf(Env &env, Tag dbl) {
    assert(IsType(env, dbl));

    return *(Heap::Layout<double>(env, dbl));
  }

  static double VSpecOf(Tag) { return 0.0; }

  static void Write(Env &, Tag, Tag, bool);

public: /* type model */
  Tag Heap(Env &env) override {
    auto alloc = env.heap->Alloc(sizeof(double), SYS_CLASS::DOUBLE);
    if (!alloc.has_value())
      throw std::runtime_error("heap exhausted");
    Layout *layout = reinterpret_cast<Layout *>(alloc.value());

    layout->dbl = layout_.dbl;
    return Entag(alloc.value(), TAG::INDIRECT);
  }

public: /* object */
  explicit Double(double dbl) : Type() { layout_.dbl = dbl; }

  ~Double() override = default;
};

} /* namespace type */
} /* namespace libmu */

#endif /* LIBMU_TYPE_FLOAT_H_ */
