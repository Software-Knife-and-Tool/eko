/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu-float.cc: library float functions
 **
 **/
#include <cmath>

#include <cassert>
#include <limits>

#include "libmu/core/context.h"
#include "libmu/core/env.h"

#include "libmu/type/exception.h"
#include "libmu/type/float.h"

namespace libmu {

using Cons = type::Cons;
using Exception = type::Exception;
using Float = type::Float;

using Context = core::Context;
using Frame = core::Context::Frame;
using Type = core::Type;

namespace mu {

/** * (float+ float float) => float **/
void FloatAdd(Context &ctx, Frame &fp) {
  Tag fl0 = fp.argv[0];
  Tag fl1 = fp.argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(ctx.env, "float+", "error", "type", fl0);
  if (!Float::IsType(fl1))
    Exception::Raise(ctx.env, "float+", "error", "type", fl1);

  fp.value = Float(Float::FloatOf(fl0) + Float::FloatOf(fl1)).tag_;
}

/** * (float- float float) => float **/
void FloatSub(Context &ctx, Frame &fp) {
  Tag fl0 = fp.argv[0];
  Tag fl1 = fp.argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(ctx.env, "float-", "error", "type", fl0);

  if (!Float::IsType(fl1))
    Exception::Raise(ctx.env, "float-", "error", "type", fl1);

  fp.value = Float(Float::FloatOf(fl0) - Float::FloatOf(fl1)).tag_;
}

/** * (float* float float) => float**/
void FloatMul(Context &ctx, Frame &fp) {
  Tag fl0 = fp.argv[0];
  Tag fl1 = fp.argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(ctx.env, "float*", "error", "type", fl0);

  if (!Float::IsType(fl1))
    Exception::Raise(ctx.env, "float*", "error", "type", fl1);

  fp.value = Float(Float::FloatOf(fl0) * Float::FloatOf(fl1)).tag_;
}

/** * (float/ float float) => float **/
void FloatDiv(Context &ctx, Frame &fp) {
  Tag fl0 = fp.argv[0];
  Tag fl1 = fp.argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(ctx.env, "float/", "error", "type", fl0);

  if (!Float::IsType(fl1))
    Exception::Raise(ctx.env, "float/", "error", "type", fl1);

  fp.value = Float(Float::FloatOf(fl0) / Float::FloatOf(fl1)).tag_;
}

/** * (float< float float) => bool **/
void FloatLessThan(Context &ctx, Frame &fp) {
  Tag fl0 = fp.argv[0];
  Tag fl1 = fp.argv[1];

  if (!Float::IsType(fl0))
    Exception::Raise(ctx.env, "float<", "error", "type", fl0);

  if (!Float::IsType(fl1))
    Exception::Raise(ctx.env, "float<", "error", "type", fl1);

  fp.value = Type::Bool(Float::FloatOf(fl0) < Float::FloatOf(fl1));
}

} /* namespace mu */
} /* namespace libmu */
