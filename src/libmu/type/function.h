/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  function.h: library functions
 **
 **/
#if !defined(LIBMU_TYPE_FUNCTION_H_)
#define LIBMU_TYPE_FUNCTION_H_

#include <cassert>
#include <functional>
#include <memory>
#include <sstream>
#include <vector>

#include "libmu/core/compile.h"
#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"

namespace libmu {

using Context = core::Context;
using Heap = core::Heap;

namespace type {

/** * function type class **/
class Function : public Type {
private:
  struct Layout {
    Tag arity;     /* arity */
    Tag form;      /* a lambda or symbol */
    Tag frame_id;  /* lexical reference */
    Tag extension; /* lambda list extensions */
  };

public:
  Layout function_;

public: /* tag */
  static bool IsType(Tag);

  static bool IsNative(Env &env, Tag fn) {
    return Symbol::IsType(form(env, fn));
  }

  /* accessors */
  static Tag arity(Env &env, Tag fn) {
    assert(IsType(fn));

    return Heap::Layout<Layout>(env, fn)->arity;
  }

  static Tag form(Env &env, Tag fn) {
    assert(IsType(fn));

    return Heap::Layout<Layout>(env, fn)->form;
  }

  static Tag set_form(Env &env, Tag fn, Tag form) {
    assert(IsType(fn));

    Heap::Layout<Layout>(env, fn)->form = form;
    return form;
  }

  static Tag frame_id(Env &env, Tag fn) {
    assert(IsType(fn));

    return Heap::Layout<Layout>(env, fn)->frame_id;
  }

  static Tag extension(Env &env, Tag fn) {
    assert(IsType(fn));

    return Heap::Layout<Layout>(env, fn)->extension;
  }

  static Tag Funcall(Env &, Tag, const std::vector<Tag> &);

  static void Gc(Env &, Tag);
  static void Write(Env &, Tag, Tag, bool);
  static Tag View(Env &, Tag);
  static Tag Clone(Env &, Tag);

public: /* type model */
  Tag Heap(Env &) override;

public: /* object */
  explicit Function() = default;

  explicit Function(Tag symbol, const Env::MuFunctionDef *mu) : Type() {
    function_.arity = Fixnum(mu->nreqs).tag_;
    function_.form = symbol;
    function_.frame_id = Fixnum(0).tag_;
    function_.extension = NIL;
  }

  explicit Function(Env &env, Tag id, Tag lambda, Tag form) : Type() {
    assert(Cons::IsList(lambda));
    assert(Cons::IsList(form));

    function_.arity = Fixnum(Cons::Length(env, lambda)).tag_;
    function_.form = form;
    function_.frame_id = id;
    function_.extension = NIL;
  }

  explicit Function(Tag id, Context *ctx) : Type() {
    (void)ctx;
    function_.arity = NIL;
    function_.form = NIL;
    function_.frame_id = id;
    function_.extension = NIL;
  }

  ~Function() override = default;
}; /* class Function */

} /* namespace type */
} /* namespace libmu */

#endif /* LIBMU_TYPE_FUNCTION_H_ */
