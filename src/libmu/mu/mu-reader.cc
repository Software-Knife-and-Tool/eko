/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu-reader.cc: library reader
 **
 **/
#include <cassert>
#include <limits>
#include <map>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/readtable.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/namespace.h"
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

/** * (read stream) => object **/
void Read(Context &ctx, Frame &fp) {
  Tag stream = fp.argv[0];

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "read", "error", "type", stream);

  if (Stream::IsEof(ctx.env, stream))
    Exception::Raise(ctx.env, "read", "error", "eof", stream);

  fp.value = Env::Read(ctx.env, stream);
}

} /* namespace mu */
} /* namespace libmu */
