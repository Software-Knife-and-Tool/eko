/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu-core.cc: library core functions
 **
 **/
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <vector>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/stream.h"
#include "libmu/type/symbol.h"

namespace libmu {

using Context = core::Context;
using Frame = core::Context::Frame;
using Type = core::Type;

using Cons = type::Cons;
using Exception = type::Exception;
using Fixnum = type::Fixnum;
using Function = type::Function;
using Symbol = type::Symbol;
using Vector = type::Vector;

namespace mu {

/** * (type-of object) => symbol **/
void TypeOf(Context &ctx, Frame &fp) {
  Tag obj = fp.argv[0];

  fp.value = Type::MapClassSymbol(Type::TypeOf(ctx.env, obj));
}

/** * (eq object object) => bool **/
void Eq(Context &, Frame &fp) {
  fp.value = Type::Bool(Type::Eq(fp.argv[0], fp.argv[1]));
}

/** * (eval object) => object **/
void Eval(Context &ctx, Frame &fp) {
  fp.value = Env::Eval(ctx, core::Compile::Form(ctx, fp.argv[0]));
}

/** * (::view object) => vector **/
void View(Context &ctx, Frame &fp) {
  fp.value = Type::View(ctx.env, fp.argv[0]);
}

/** * (::compile form env) => object **/
void Compile(Context &ctx, Frame &fp) {
  Tag form = fp.argv[0];

  fp.value = core::Compile::Form(ctx, form);
}

} /* namespace mu */
} /* namespace libmu */
