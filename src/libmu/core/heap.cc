/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  heap.cc: heap interface
 **
 **/
#include <cassert>
#include <cinttypes>
#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/char.h"
#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/stream.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

namespace libmu {
namespace core {

/** * convert tag to uint64_t **/
void *Heap::LayoutAddr(Env &env, Tag ptr) {
  return env.heap->HeapAddr(std::to_underlying(ptr) & ~0x7);
}

/** * garbage collection **/
void Heap::GcMark(Env &env, Tag ptr) {

  std::function<void(Env &, Tag)> noGc = [](Env &, Tag) {};
  static const std::map<SYS_CLASS, std::function<void(Env &, Tag)>> kGcTypeMap{
      {SYS_CLASS::CONS, type::Cons::GcMark},
      {SYS_CLASS::EXCEPTION, type::Exception::GcMark},
      {SYS_CLASS::DOUBLE, noGc},
      {SYS_CLASS::FIXNUM, noGc},
      {SYS_CLASS::FLOAT, noGc},
      {SYS_CLASS::FUNCTION, type::Function::GcMark},
      {SYS_CLASS::NAMESPACE, type::Namespace::GcMark},
      {SYS_CLASS::STREAM, type::Stream::GcMark},
      {SYS_CLASS::SYMBOL, type::Symbol::GcMark},
      {SYS_CLASS::VECTOR, type::Vector::GcMark}};

  assert(kGcTypeMap.contains(Type::TypeOf(env, ptr)));
  kGcTypeMap.at(Type::TypeOf(env, ptr))(env, ptr);
}

bool Heap::Gc(Env &env) {
  env.heap->ClearRefBits();

  for (auto &ns : env.namespaces) {
    const auto [name, nspc] = ns;
    GcMark(env, nspc);
  }

  return true;
}

/** * heap object marked? **/
bool Heap::IsGcMarked(Env &env, Type::Tag ptr) {
  HeapInfo *hinfo = env.heap->GetHeapInfo(ptr);

  return RefBits(*hinfo) == 0 ? false : true;
}

} /* namespace core */
} /* namespace libmu */
