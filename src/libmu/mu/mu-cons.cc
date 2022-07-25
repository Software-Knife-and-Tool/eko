/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu-cons.cc: library list functions
 **
 **/
#include <cassert>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/vector.h"

namespace libmu {

using Cons = type::Cons;
using Exception = type::Exception;
using Fixnum = type::Fixnum;
using Frame = core::Context::Frame;
using Function = type::Function;
using Symbol = type::Symbol;
using Vector = type::Vector;

using Context = core::Context;
using Env = core::Env;
using Frame = core::Context::Frame;
using Tag = core::Type::Tag;
using Type = core::Type;

namespace mu {
/** * (length list) => fixnum **/
void ListLength(Context &ctx, Frame &fp) {
  Tag list = fp.argv[0];

  if (!Cons::IsList(list))
    Exception::Raise(ctx.env, "length", "error", "type", list);

  fp.value = Fixnum(Cons::Length(ctx.env, list)).tag_;
}

/** * (cons object object) => cons **/
void Cons(Context &ctx, Frame &fp) {
  fp.value = type::Cons(fp.argv[0], fp.argv[1]).Heap(ctx.env);
}

/** * (car list) => object **/
void Car(Context &ctx, Frame &fp) {
  Tag list = fp.argv[0];

  if (!Cons::IsList(list))
    Exception::Raise(ctx.env, "car", "error", "type", list);

  fp.value = Cons::car(ctx.env, list);
}

/** * (cdr list) => object **/
void Cdr(Context &ctx, Frame &fp) {
  Tag list = fp.argv[0];

  if (!Cons::IsList(list))
    Exception::Raise(ctx.env, "cdr", "error", "type", list);

  fp.value = Cons::cdr(ctx.env, list);
}

/** * (nth fixnum list) => object **/
void Nth(Context &ctx, Frame &fp) {
  Tag nth = fp.argv[0];
  Tag list = fp.argv[1];

  if (!Fixnum::IsType(nth))
    Exception::Raise(ctx.env, "nth", "error", "type", nth);

  if (Fixnum::Int64Of(nth) < 0)
    Exception::Raise(ctx.env, "nth", "error", "type", nth);

  if (!Cons::IsList(list))
    Exception::Raise(ctx.env, "nth", "error", "type", list);

  fp.value = Cons::Nth(ctx.env, list, Fixnum::Int64Of(nth));
}

/** * (nthcdr fixnum list) => object **/
void Nthcdr(Context &ctx, Frame &fp) {
  Tag index = fp.argv[0];
  Tag list = fp.argv[1];

  if (!Fixnum::IsType(index) || Fixnum::Int64Of(index) < 0)
    Exception::Raise(ctx.env, "nthcdr", "error", "type", index);

  if (!Cons::IsList(list))
    Exception::Raise(ctx.env, "nthcdr", "error", "type", list);

  uint64_t nth = Fixnum::Int64Of(index);

  if (!Cons::IsList(Cons::cdr(ctx.env, list))) {
    if (nth > 1)
      Exception::Raise(ctx.env, "nthcdr", "error", "range", index);

    fp.value = nth == 0 ? Cons::car(ctx.env, list) : Cons::cdr(ctx.env, list);
    return;
  }

  if (nth == 0 || Type::Null(list)) {
    fp.value = list;
  } else {
    fp.value = Type::NIL;

    Cons::iter iter(ctx.env, list);
    for (auto it = iter.begin(); it != iter.end(); it = ++iter)
      if (--nth == 0) {
        fp.value = Cons::cdr(ctx.env, it);
        return;
      }
  }
}

} /* namespace mu */
} /* namespace libmu */
