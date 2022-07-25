/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu-exception.cc: library exception functions
 **
 **/
#include <cassert>
#include <iostream>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/exception.h"
#include "libmu/type/function.h"
#include "libmu/type/namespace.h"

namespace libmu {

using Context = core::Context;

using Cons = type::Cons;
using Exception = type::Exception;
using Frame = core::Context::Frame;
using Function = type::Function;
using Symbol = type::Symbol;

namespace mu {
/** * (raise-exception exception) **/
[[noreturn]] void RaiseException(Context &ctx, Frame &fp) {
  Tag exception = fp.argv[0];

  if (!Exception::IsType(exception))
    Exception::Raise(ctx.env, "ex-rais", "error", "type", exception);

  fp.value = exception;
  Exception::RaiseException(ctx.env, exception);
}

/** * (exception tag source) **/
void Exception(Context &ctx, Frame &fp) {
  Tag tag = fp.argv[0];
  Tag eclass = fp.argv[1];
  Tag etype = fp.argv[2];
  Tag source = fp.argv[3];

  if (!Symbol::IsKeyword(tag))
    Exception::Raise(ctx.env, "except", "error", "type", tag);

  if (!Symbol::IsKeyword(eclass))
    Exception::Raise(ctx.env, "except", "error", "type", eclass);

  if (!Symbol::IsKeyword(etype))
    Exception::Raise(ctx.env, "except", "error", "type", etype);

  fp.value = type::Exception(tag, eclass, etype, source).Heap(ctx.env);
}

/** * (with-exception func func) **/
void WithException(Context &ctx, Frame &fp) {
  Tag handler = fp.argv[0];
  Tag thunk = fp.argv[1];

  auto mark = ctx.dynamic.size();

  if (!Function::IsType(thunk))
    Exception::Raise(ctx.env, "with-ex", "error", "type", thunk);

  if (!Function::IsType(handler))
    Exception::Raise(ctx.env, "with-ex", "error", "type", handler);

  Type::Tag value;

  try {
    value = Env::Funcall(ctx, thunk, std::vector<Type::Tag>{});
  } catch (Type::Tag &ex) {
    ctx.dynamic.resize(mark);
    value = Env::Funcall(ctx, handler, std::vector<Type::Tag>{ex});
  } catch (const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
    throw std::runtime_error("unexpected throw from libmu");
  }

  fp.value = value;
}

} /* namespace mu */
} /* namespace libmu */
