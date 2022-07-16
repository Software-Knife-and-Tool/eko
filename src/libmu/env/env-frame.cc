/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  env-frame.cc: library env
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

/** * (lex-ref fixnum fixnum) => object **/
void FrameRef(Context &ctx, Frame &fp) {
  Tag frame_id = fp.argv[0];
  Tag offset = fp.argv[1];

  if (!Fixnum::IsType(offset))
    Exception::Raise(ctx.env, "lex-ref", "error", "type", offset);

  std::optional<Frame *> lfp = ctx.MapLexicalContext(frame_id, 0);

  if (!lfp.has_value())
    throw std::runtime_error("frame ref botch");

  fp.value = lfp.value()->argv[Fixnum::Int64Of(offset)];
}

/** * (fnv-lex fn) => frame **/
void FrameGet(Context &ctx, Frame &fp) {
  std::vector<Tag> args{};

  Tag fn = fp.argv[0];

  if (!Function::IsType(ctx.env, fn))
    Exception::Raise(ctx.env, "fnv-lex", "error", "type", fn);

  std::optional<Frame *> fstate =
      ctx.MapLexicalContext(Function::frame_id(ctx.env, fn), 0);

  if (!fstate.has_value())
    Exception::Raise(ctx.env, "fnv-lex", "error", "unbound", fn);

  fp.value = ctx.FrameToTag(*(fstate.value()));
}

/** * (fnv-set frame offset value) => old-value **/
void FrameSetVar(Context &ctx, Frame &fp) {

  Tag fr = fp.argv[0];
  Tag off = fp.argv[1];
  Tag value = fp.argv[2];

  if (!Cons::IsType(ctx.env, fr))
    Exception::Raise(ctx.env, "fnv-set", "error", "type", fr);

  if (!Fixnum::IsType(off))
    Exception::Raise(ctx.env, "fnv-set", "error", "type", off);

  fp.value = ctx.NthArgOf(fr, Fixnum::Int64Of(off));
  ctx.SetNthArgOf(fr, value, Fixnum::Int64Of(off));
}

/** * (lex-set fn offset value) => old-value **/
void FrameSet(Context &ctx, Frame &fp) {

  Tag fn = fp.argv[0];
  Tag off = fp.argv[1];
  Tag value = fp.argv[2];

  if (!Function::IsType(ctx.env, fn))
    Exception::Raise(ctx.env, "lex-set", "error", "type", fn);

  if (!Fixnum::IsType(off))
    Exception::Raise(ctx.env, "lex-set", "error", "type", off);

  auto frame = ctx.MapLexicalContext(Function::frame_id(ctx.env, fn), 0);

  if (!frame.has_value())
    throw std::runtime_error("frame ref botch");

  fp.value = frame.value()->argv[Fixnum::Int64Of(off)];

  frame.value()->argv[Fixnum::Int64Of(off)] = value;
}

/** * (lex-psh frame) => frame **/
void FramePush(Context &ctx, Frame &fp) {
  Tag fr = fp.argv[0];

  if (!Cons::IsType(ctx.env, fr))
    Exception::Raise(ctx.env, "fr-push", "error", "type", fr);

  Frame *frame = ctx.TagToFrame(fr);

  ctx.closures[fr].push_back(frame);
  ctx.lexical[Function::frame_id(ctx.env, frame->func)].push_back(frame);
  fp.value = fr;
}

/** * (lex-pop frame) => frame **/
void FramePop(Context &ctx, Frame &fp) {
  Tag fr = fp.argv[0];

  if (!Cons::IsType(ctx.env, fr))
    Exception::Raise(ctx.env, "fr-pop", "error", "type", fr);

  delete ctx.closures[fr].back();
  ctx.closures[fr].pop_back();
  ctx.lexical[Function::frame_id(ctx.env, ctx.FuncIdOf(fr))].pop_back();

  fp.value = fr;
}

} /* namespace env */
} /* namespace libmu */
