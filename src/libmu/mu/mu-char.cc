/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu-char.cc: library list functions
 **
 **/
#include <cassert>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/char.h"
#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/vector.h"

namespace libmu {

using Char = type::Char;
using Cons = type::Cons;
using Exception = type::Exception;
using Fixnum = type::Fixnum;
using Function = type::Function;
using Symbol = type::Symbol;
using Vector = type::Vector;

using Context = core::Context;
using Env = core::Env;
using Frame = core::Context::Frame;
using Tag = core::Type::Tag;
using Type = core::Type;

namespace mu {

/** * (ch-code char) => fixnum **/
void CharCode(Context &ctx, Frame &fp) {
  Tag ch = fp.argv[0];

  if (!Char::IsType(ch))
    Exception::Raise(ctx.env, "ch-code", "error", "type", ch);

  fp.value = Fixnum(size_t{Char::UInt64Of(ch)}).tag_;
};

/** * (code-ch fixnum) => ch **/
void CodeChar(Context &ctx, Frame &fp) {
  Tag fx = fp.argv[0];

  if (!Fixnum::IsType(fx))
    Exception::Raise(ctx.env, "code-ch", "error", "type", fx);

  fp.value = Char(Fixnum::Int64Of(fx)).tag_;
}

} /* namespace mu */
} /* namespace libmu */
