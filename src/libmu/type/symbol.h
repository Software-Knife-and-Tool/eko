/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  symbol.h: library symbols
 **
 **/
#if !defined(LIBMU_TYPE_SYMBOL_H_)
#define LIBMU_TYPE_SYMBOL_H_

#include <cassert>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/readtable.h"
#include "libmu/core/type.h"

#include "libmu/type/stream.h"
#include "libmu/type/vector.h"

namespace libmu {

using Heap = core::Heap;

namespace type {

/** * symbol type class **/
class Symbol : public Type {
private:
  typedef struct {
    Tag ns;
    Tag name;
    Tag value;
  } Layout;

  Heap::HeapInfo heapinfo_ =
      Heap::MakeHeapInfo(sizeof(Layout), SYS_CLASS::SYMBOL);
  Layout symbol_;

public: /* tag */
  Heap::HeapInfo heapinfo() { return heapinfo_; }

  static bool IsType(Tag);

  /* Keywords */
  static inline bool IsKeyword(Tag ptr) {
    return IsDirect(ptr) && (DirectClass(ptr) == DIRECT_CLASS::SYMBOL);
  }

  static Tag Keyword(Tag);
  static Tag Keyword(const std::string &);

  /* accessors */
  static Tag ns(Env &, Tag);

  static Tag value(Env &env, Tag symbol) {
    assert(IsType(symbol));

    return IsKeyword(symbol) ? symbol
                             : Heap::Layout<Layout>(env, symbol)->value;
  }

  static Tag set_value(Env &env, Tag symbol, Tag value) {
    assert(IsType(symbol));

    return IsKeyword(symbol) ? symbol
                             : Heap::Layout<Layout>(env, symbol)->value = value;
  }

  static Tag name(Env &env, Tag symbol) {
    assert(IsType(symbol));

    return IsKeyword(symbol)
               ? MakeDirect(DirectData(symbol), DirectSize(symbol),
                            DIRECT_CLASS::VECTOR)
               : Heap::Layout<Layout>(env, symbol)->name;
  }

  static void Gc(Env &, Tag);
  static bool IsBound(Env &, Tag);
  static bool IsUninterned(Env &, Tag);
  static void Write(Env &, Tag, Tag, bool);
  static Tag ParseSymbol(Env &, std::string);

public: /* type model */
  Tag tag_;

  Tag Heap(Env &) override;

public: /* object */
  explicit Symbol(Tag, Tag, Tag);

  ~Symbol() override = default;
};

} /* namespace type */
} /* namespace libmu */

#endif /* LIBMU_TYPE_SYMBOL_H_ */
