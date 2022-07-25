/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  core-context.cc: execution contexts
 **
 **/
#include "libmu/core/context.h"

#include <cassert>
#include <condition_variable>
#include <functional>
#include <iomanip>
#include <map>
#include <mutex>
#include <vector>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
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
using Namespace = type::Namespace;
using Stream = type::Stream;
using Symbol = type::Symbol;
using Vector = type::Vector;

namespace core {

Tag Context::FrameToTag(Frame &fp) {
  int64_t asize = Fixnum::Int64Of(Function::arity(env, fp.func));

  std::vector<Tag> argv(fp.argv, fp.argv + asize);
  return Cons(fp.func, Vector(argv).Heap(env)).Heap(env);
}

Tag Context::FuncIdOf(Tag frame) {
  assert(Cons::IsType(frame));

  return Cons::car(env, frame);
}

Tag Context::NthArgOf(Tag frame, int nth) {
  assert(Cons::IsType(frame));

  Tag argv = Cons::cdr(env, frame);
  assert(Vector::IsTyped(env, argv, SYS_CLASS::T));

  return Vector::Ref<Tag>(env, argv, nth);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void Context::SetNthArgOf(Tag frame, Tag value, int nth) {
  assert(Cons::IsType(frame));

  Tag argv = Cons::cdr(env, frame);
  assert(Vector::IsTyped(env, argv, SYS_CLASS::T));

  Vector::Data<Tag>(env, argv)[nth] = value;
}

Context::Frame *Context::TagToFrame(Tag frame) {
  assert(Cons::IsType(frame));

  Tag func = Cons::car(env, frame);
  assert(Function::IsType(func));

  Tag argv = Cons::cdr(env, frame);
  assert(Vector::IsTyped(env, argv, SYS_CLASS::T));

  return new Frame(func, Vector::Data<Tag>(env, argv));
}

Tag Context::CaptureContext() {

  std::vector<Tag> dynamic_{};
  for (auto fr : dynamic)
    dynamic_.push_back(FrameToTag(*fr));

  std::vector<Tag> lexical_{};

#if 0
  for (auto lp : lexical) {
    const auto [fid, symbols] = lp;
    if (symbols.size()) {
      std::vector<Tag> frame{};
      std::vector<Tag> frames{};

      frame.push_back(fid);
      for (auto fr : symbols)
        frames.push_back(FrameToTag(*fr));

      frame.push_back(Vector(frames).Heap(env));

      lexical_.push_back(Vector(frame).Heap(env));
    }
  }
#endif

  std::vector<Tag> context{};
  context.push_back(Cons::List(env, dynamic_));
  context.push_back(Vector(lexical_).Heap(env));

  return Cons::List(env, context);
}

void Context::GcFrame(Env &env, Frame &fp) {
  Env::Gc(env, fp.func);

  int64_t nargs = Fixnum::Int64Of(Function::arity(env, fp.func));
  for (int i = 0; i < nargs; ++i)
    Env::Gc(env, fp.argv[i]);
}

void Context::GcContext() {
#if 0
    fp->env.GcMark(fp->func);

    int64_t nargs = Fixnum::Int64Of(Function::arity(fp->env, fp->func));
    for (int64_t i = 0; i < nargs; ++i)
      fp->env.GcMark(fp->argv[i]);
#endif
}

/** * constructors **/
Context::Context(Env &env, Tag ctx) : env(env) {
  Tag dynamic_ = Cons::Nth(env, ctx, 0);
  Tag lexical_ = Cons::Nth(env, ctx, 1);

  if (Type::Null(ctx))
    return;

  Vector::iter<Tag> diter(env, dynamic_);
  for (auto it = diter.begin(); it != diter.end(); it = ++diter)
    dynamic.push_back(TagToFrame(*it));

  Vector::iter<Tag> liter(env, lexical_);
  for (auto it = liter.begin(); it != liter.end(); it = ++liter) {
    Tag fid = Vector::Ref<Tag>(env, *it, 0);
    Tag stack = Vector::Ref<Tag>(env, *it, 1);

    std::vector<Frame *> ctx_stack{};

    Vector::iter<Tag> siter(env, stack);
    for (auto sit = siter.begin(); sit != siter.end(); sit = ++siter)
      ctx_stack.push_back(TagToFrame(*sit));

    lexical.insert(std::pair<Tag, std::vector<Frame *>>{fid, ctx_stack});
  }
}

} /* namespace core */
} /* namespace libmu */
