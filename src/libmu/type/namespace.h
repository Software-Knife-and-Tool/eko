/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  namespace.h: library symbol bindings
 **
 **/
#if !defined(LIBMU_TYPE_NAMESPACE_H_)
#define LIBMU_TYPE_NAMESPACE_H_

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/stream.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

namespace libmu {

using Heap = core::Heap;

namespace type {

/** * namespace type class **/
class Namespace : public Type {
private:
  typedef struct {
    Tag name;    /* string */
    Tag import;  /* import ns */
    Tag externs; /* list of externs */
    Tag interns; /* list of interns */
  } Layout;

  Layout namespace_;

public: /* tag */
  enum class SCOPE : bool { EXTERN, INTERN };

  static bool IsType(Env &, Tag);

  static Tag NsDesignator(Env &, Tag);

  /** * accessors **/
  static Tag name(Env &env, Tag ns) {
    assert(IsType(env, ns));

    return Heap::Layout<Layout>(env, ns)->name;
  }

  static Tag import(Env &env, Tag ns) {
    assert(IsType(env, ns));

    return Heap::Layout<Layout>(env, ns)->import;
  }

  static Tag externs(Env &env, Tag ns) {
    assert(IsType(env, ns));

    return Heap::Layout<Layout>(env, ns)->externs;
  }

  static Tag interns(Env &env, Tag ns) {
    assert(IsType(env, ns));

    return Heap::Layout<Layout>(env, ns)->interns;
  }

  static void externs(Env &env, Tag ns, Tag value) {
    assert(IsType(env, ns));

    Heap::Layout<Layout>(env, ns)->externs = value;
  }

  static void interns(Env &env, Tag ns, Tag value) {
    assert(IsType(env, ns));

    Heap::Layout<Layout>(env, ns)->interns = value;
  }

  static Tag Intern(Env &, Tag, SCOPE, Tag, Tag);
  static std::optional<Tag> Map(Env &, Tag, SCOPE, Tag);

  static void Gc(Env &, Tag);
  static void Write(Env &, Tag, Tag, bool);
  static Tag Symbols(Env &, Tag);
  static Tag View(Env &, Tag);

public: /* type model */
  Tag Heap(Env &) override;

public: /* object */
  explicit Namespace() = default;
  explicit Namespace(Tag, Tag);
  explicit Namespace(Env &, Tag, Tag);

  ~Namespace() override = default;
}; /* class Namespace */

} /* namespace type */
} /* namespace libmu */

#endif /* LIBMU_TYPE_NAMESPACE_H_ */
