/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  env-context.cc: library env
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

namespace env {

/** * (cx-info) => list **/
void ContextInfo(Context &ctx, Frame &fp) {
  std::vector<Tag> stack;

  stack.push_back(Fixnum(ctx.stack_info.calls).tag_);
  stack.push_back(Fixnum(ctx.stack_info.current).tag_);
  stack.push_back(Fixnum(ctx.stack_info.max).tag_);

  fp.value = Cons::List(ctx.env, stack);
}

/** * (context) => ctx **/
void GetContext(Context &ctx, Frame &fp) { fp.value = ctx.CaptureContext(); }

/** * (::bind fn ctx) => bool **/
void BindContext(Context &ctx, Frame &fp) {
  Tag fn = fp.argv[0];
  Tag vctx = fp.argv[1];

  if (!Function::IsType(fn))
    Exception::Raise(ctx.env, "bind", "error", "type", fn);

  if (!Cons::IsList(vctx))
    Exception::Raise(ctx.env, "bind", "error", "type", vctx);

  ctx.thread = std::make_unique<std::thread>([&ctx, &fn]() {
    ctx.Suspend();

    Env::Funcall(ctx, fn, std::vector<Tag>{});
  });

  // Context* nctx = new Context(ctx.env, thread->get_id(), vctx);

  fp.value = Function(ctx.env.compiler->frameid(), &ctx).Heap(ctx.env);
}

/** * (::resume ctx) => bool **/
void ResumeContext(Context &ctx, Frame &fp) {
  Tag vctx = fp.argv[0];

  if (!Cons::IsType(vctx))
    Exception::Raise(ctx.env, "resume", "error", "type", vctx);

  fp.value = Type::T;
}

/** * (::suspend vector) => bool **/
void SuspendContext(Context &, Frame &fp) { fp.value = fp.argv[0]; }

/** * (fix fn) => frame **/
void ContextFix(Context &ctx, Frame &fp) {
  fp.value = fp.argv[0];

  if (!Function::IsType(fp.value))
    Exception::Raise(ctx.env, "ctx-fix", "error", "type", fp.value);

  std::vector<Tag> noargs{};

  for (;;) {
    fp.value = Env::Funcall(ctx, fp.value, noargs);
    if (!type::Function::IsType(fp.value))
      break;
  }
}

} /* namespace env */
} /* namespace libmu */
