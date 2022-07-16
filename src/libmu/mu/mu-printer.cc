/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu-printer.cc: library print functions
 **
 **/
#include <bitset>
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <utility>

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
#include "libmu/type/vector.h"

namespace libmu {

using Cons = type::Cons;
using Exception = type::Exception;
using Fixnum = type::Fixnum;
using Float = type::Float;
using Function = type::Function;
using Stream = type::Stream;
using Symbol = type::Symbol;
using Vector = type::Vector;

using Context = core::Context;
using Env = core::Env;
using Frame = core::Context::Frame;
using Tag = core::Tag;
using Type = core::Type;

namespace mu {

/** * (write object stream bool) => object **/
void Write(Context &ctx, Frame &fp) {
  Tag obj = fp.argv[0];
  Tag stream = fp.argv[1];
  Tag escape = fp.argv[2];

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "write", "error", "type", stream);

  Env::Write(ctx.env, obj, stream, !Type::Null(escape));
  fp.value = obj;
}

} /* namespace mu */
} /* namespace libmu */
