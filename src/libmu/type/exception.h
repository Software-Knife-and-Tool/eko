/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  exception.h: libmu exceptions
 **
 **/
#if !defined(LIBMU_TYPE_EXCEPTION_H_)
#define LIBMU_TYPE_EXCEPTION_H_

#include <cassert>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/symbol.h"

namespace libmu {

using Heap = core::Heap;

namespace type {

/** * exception type class **/
class Exception : public Type {
private:
  typedef struct {
    Tag eclass; /* execption class */
    Tag etype;  /* exception tag */
    Tag source; /* on source object */
    Tag frame;  /* frame */
    Tag tag;    /* tag */
  } Layout;

  Layout exception_;

public:
  static inline bool IsType(Env &env, Tag ptr) {
    return IsIndirect(ptr) &&
           env.heap->SysClass(env, ptr) == SYS_CLASS::EXCEPTION;
  }

  /** * accessors **/
  static Tag tag(Env &env, Tag exception) {
    assert(IsType(env, exception));

    return Heap::Layout<Layout>(env, exception)->tag;
  }

  static Tag frame(Env &env, Tag exception) {
    assert(IsType(env, exception));

    return Heap::Layout<Layout>(env, exception)->frame;
  }

  static Tag eclass(Env &env, Tag exception) {
    assert(IsType(env, exception));

    return Heap::Layout<Layout>(env, exception)->eclass;
  }

  static Tag etype(Env &env, Tag exception) {
    assert(IsType(env, exception));

    return Heap::Layout<Layout>(env, exception)->etype;
  }

  static Tag source(Env &env, Tag exception) {
    assert(IsType(env, exception));

    return Heap::Layout<Layout>(env, exception)->source;
  }

public: /* type model */
  Tag Heap(Env &) override;

public: /* object */
  static void GcMark(Env &, Tag);
  static Tag View(Env &, Tag);

  [[noreturn]] static void Raise(Env &, const std::string &,
                                 const std::string &, const std::string &, Tag);
  [[noreturn]] static void RaiseException(Env &, Tag);

  explicit Exception() = default;
  explicit Exception(const std::string &tag, const std::string &eclass,
                     const std::string &etype, Tag source)
      : Type() {
    exception_.eclass = Symbol::Keyword(eclass);
    exception_.etype = Symbol::Keyword(etype);
    exception_.source = source;
    exception_.tag = Symbol::Keyword(tag);
  }

  // NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
  explicit Exception(Tag tag, Tag eclass, Tag etype, Tag source) : Type() {
    assert(Symbol::IsKeyword(tag));
    assert(Symbol::IsKeyword(eclass));
    assert(Symbol::IsKeyword(etype));

    exception_.eclass = eclass;
    exception_.etype = etype;
    exception_.source = source;
    exception_.tag = tag;
  }

  ~Exception() override = default;
};

} /* namespace type */
} /* namespace libmu */

#endif /* LIBMU_TYPE_EXCEPTION_H_ */
