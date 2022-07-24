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
#include <optional>
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
void Heap::Gc(Env &env) {

  env.heap->nfree = 0;
  env.heap->free_lists.clear();
  env.heap->Map([](HeapInfo *it) { *it = RefBits(*it, 0); });
}

void Heap::GcMark(Env &env, Tag ptr) {
  HeapInfo *hinfo = env.heap->Map(ptr);

  *hinfo = RefBits(*hinfo, 1);
  env.heap->nfree = 0;
}

void Heap::GcSweep(Env &env) {

  env.heap->Map([&env](HeapInfo *it) {
    if (RefBits(*it) == 0) {
      env.heap->free_lists[SysClass(*it)].push_back(it);
      env.heap->nfree++;
    }
  });
}

bool Heap::IsGcMarked(Env &env, Type::Tag ptr) {
  HeapInfo *hinfo = env.heap->Map(ptr);

  return RefBits(*hinfo) == 0 ? false : true;
}

std::optional<size_t> Heap::GcAlloc(Env &env, int size,
                                    Type::SYS_CLASS sys_class) {

  if (env.heap->free_lists.contains(sys_class) &&
      env.heap->free_lists[sys_class].size()) {
    HeapInfo *free = env.heap->free_lists[sys_class].back();

    env.heap->free_lists[sys_class].pop_back();
    return env.heap->HeapInfoTag(free);
  }

  return env.heap->Alloc(size, sys_class);
}

} /* namespace core */
} /* namespace libmu */
