/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu-fixnum.cc: library fixnums
 **
 **/
#include <cassert>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"

namespace libmu {

using Cons = type::Cons;
using Exception = type::Exception;
using Fixnum = type::Fixnum;

using Context = core::Context;
using Frame = core::Context::Frame;

namespace mu {

/** * (fixnum+ fixnum fixnum) => fixnum **/
void FixAdd(Context &ctx, Frame &fp) {
  Tag fx0 = fp.argv[0];
  Tag fx1 = fp.argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(ctx.env, "fixnum+", "error", "type", fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(ctx.env, "fixnum+", "error", "type", fx1);

  fp.value = Fixnum(Fixnum::Int64Of(fx0) + Fixnum::Int64Of(fx1)).tag_;
}

/** * (fixnum- fixnum fixnum) => fixnum **/
void FixSub(Context &ctx, Frame &fp) {
  Tag fx0 = fp.argv[0];
  Tag fx1 = fp.argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(ctx.env, "fixnum-", "error", "type", fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(ctx.env, "fixnum-", "error", "type", fx1);

  fp.value = Fixnum(Fixnum::Int64Of(fx0) - Fixnum::Int64Of(fx1)).tag_;
}

/** * (fixnum* fixnum fixnum) => fixnum **/
void FixMul(Context &ctx, Frame &fp) {
  Tag fx0 = fp.argv[0];
  Tag fx1 = fp.argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(ctx.env, "fixnum*", "error", "type", fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(ctx.env, "fixnum*", "error", "type", fx1);

  fp.value = Fixnum(Fixnum::Int64Of(fx0) * Fixnum::Int64Of(fx1)).tag_;
}

/** * (fixnum< fixnum fixnum) => bool **/
void FixLessThan(Context &ctx, Frame &fp) {
  Tag fx0 = fp.argv[0];
  Tag fx1 = fp.argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(ctx.env, "fixnum<", "error", "type", fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(ctx.env, "fixnum<", "error", "type", fx1);

  fp.value = Type::Bool(Fixnum::Int64Of(fx0) < Fixnum::Int64Of(fx1));
}

/** * (truncate fixnum fixnum) => (fixnum . fixnum) **/
void Truncate(Context &ctx, Frame &fp) {
  Tag fx0 = fp.argv[0];
  Tag fx1 = fp.argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(ctx.env, "trunc", "error", "type", fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(ctx.env, "trunc", "error", "type", fx1);

  if (Fixnum::Int64Of(fx1) == 0)
    Exception::Raise(ctx.env, "trunc", "error", "type", fx1);

  int64_t ifx0 = Fixnum::Int64Of(fx0);
  int64_t ifx1 = Fixnum::Int64Of(fx1);
  int64_t quot = ifx0 < ifx1 ? 0 : ifx0 / ifx1;
  int64_t rem = quot == 0 ? ifx0 : ifx0 - (ifx1 * quot);

  fp.value = Cons(Fixnum(quot).tag_, Fixnum(rem).tag_).Heap(ctx.env);
}

/** * (floor fixnum fixnum) => (fixnum . fixnum) **/
void Floor(Context &ctx, Frame &fp) {
  Tag number = fp.argv[0];
  Tag divisor = fp.argv[1];

  /* quotient * divisor + remainder = number */
  if (!Fixnum::IsType(number))
    Exception::Raise(ctx.env, "floor", "error", "type", number);

  if (!Fixnum::IsType(divisor))
    Exception::Raise(ctx.env, "floor", "error", "type", divisor);

  if (Fixnum::Int64Of(divisor) == 0)
    Exception::Raise(ctx.env, "floor", "error", "type", divisor);

  int64_t nx = Fixnum::Int64Of(number);
  int64_t dx = Fixnum::Int64Of(divisor);

  int64_t rem = nx - (nx / dx) * dx;
  int64_t quot = (nx - rem) / dx;

  fp.value = Cons(Fixnum(quot).tag_, Fixnum(rem).tag_).Heap(ctx.env);
}

/** * (logand fixnum fixnum) => fixnum **/
void Logand(Context &ctx, Frame &fp) {
  Tag fx0 = fp.argv[0];
  Tag fx1 = fp.argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(ctx.env, "logand", "error", "type", fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(ctx.env, "logand", "error", "type", fx1);

  fp.value = Fixnum(Fixnum::Int64Of(fx0) & Fixnum::Int64Of(fx1)).tag_;
}

/** * (logor fixnum fixnum) => fixnum **/
void Logor(Context &ctx, Frame &fp) {
  Tag fx0 = fp.argv[0];
  Tag fx1 = fp.argv[1];

  if (!Fixnum::IsType(fx0))
    Exception::Raise(ctx.env, "logor", "error", "type", fx0);

  if (!Fixnum::IsType(fx1))
    Exception::Raise(ctx.env, "logor", "error", "type", fx1);

  fp.value = Fixnum(Fixnum::Int64Of(fx0) | Fixnum::Int64Of(fx1)).tag_;
}

} /* namespace mu */
} /* namespace libmu */
