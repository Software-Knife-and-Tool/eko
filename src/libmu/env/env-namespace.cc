/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  env-namespace.cc: library env
 **
 **/
#include <algorithm>
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
using Namespace = type::Namespace;
using Symbol = type::Symbol;
using Vector = type::Vector;

using SCOPE = Namespace::SCOPE;

using Context = core::Context;
using Env = core::Env;
using Frame = core::Context::Frame;
using Tag = core::Type::Tag;
using Type = core::Type;

namespace env {

/** * (find-ns string) => namespace **/
void MapNamespace(Context &ctx, Frame &fp) {
  Tag name = fp.argv[0];

  if (!Vector::IsTyped(ctx.env, name, Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, "find-ns", "error", "type", name);

  std::optional<Tag> ns =
      ctx.env.MapNamespace(Vector::StdStringOf(ctx.env, name));

  fp.value = ns.has_value() ? ns.value() : Type::NIL;
}

/** * (ns ns) => namespace **/
void Namespace(Context &ctx, Frame &fp) {
  Tag ns = fp.argv[0];

  if (!Namespace::IsType(ns))
    Exception::Raise(ctx.env, "ns", "error", "type", ns);

  fp.value = ns;

  if (Type::Null(ns))
    return;

  std::optional<Tag> map_ns = ctx.env.MapNamespace(
      Vector::StdStringOf(ctx.env, type::Namespace::name(ctx.env, ns)));

  if (map_ns.has_value())
    fp.value = map_ns.value();
  else
    ctx.env.AddNamespace(ns);
}

} /* namespace env */
} /* namespace libmu */
