/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  exception.cc: libmu exceptions
 **
 **/
#include "libmu/type/exception.h"

#include <cassert>
#include <iostream>
#include <map>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/function.h"
#include "libmu/type/namespace.h"

namespace libmu {

using Tag = core::Type::Tag;
using Heap = core::Heap;

namespace type {
Tag Exception::Heap(Env &env) {
  auto alloc = env.heap->Alloc(sizeof(Layout), SYS_CLASS::EXCEPTION);

  if (!alloc.has_value())
    throw std::runtime_error("heap exhausted");

  Tag tag = Entag(alloc.value(), TAG::INDIRECT);
  *env.heap->Layout<Layout>(env, tag) = exception_;

  return tag;
}

/** * garbage collection **/
void Exception::Gc(Env &env, Tag exception) {
  assert(IsType(env, exception));

  if (Env::IsGcMarked(env, exception))
    return;

  Env::GcMark(env, exception);

  Env::Gc(env, source(env, exception));
  Env::Gc(env, tag(env, exception));
}

/** * make view of exception **/
Tag Exception::View(Env &env, Tag ex) {
  assert(IsType(env, ex));

  std::vector<Tag> view = std::vector<Tag>{tag(env, ex), eclass(env, ex),
                                           etype(env, ex), source(env, ex)};

  return Vector(view).Heap(env);
}

/** * raise exception **/
void Exception::RaiseException(Env &env, Tag exception) {
  assert(IsType(env, exception));

  (void)env;
  throw exception;
}

/** * raise exception (internal) **/
void Exception::Raise(Env &env, const std::string &tag,
                      const std::string &eclass, const std::string &etype,
                      Tag source) {

  RaiseException(env, Exception(tag, eclass, etype, source).Heap(env));
}

} /* namespace type */
} /* namespace libmu */
