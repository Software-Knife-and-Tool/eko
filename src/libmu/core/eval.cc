/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  eval.cc: library eval/apply
 **
 **/
#include <array>
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <vector>

#include "libmu/core/context.h"
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

using Frame = Context::Frame;

/** * call function with argument vector **/
Tag Env::Funcall(Context &ctx, Tag func, const std::vector<Tag> &argv) {
  assert(Function::IsType(func));

  Env &env = ctx.env;
  size_t nreqs = Fixnum::Int64Of(Function::arity(env, func));

  if (nreqs != argv.size())
    Exception::Raise(env, "funcall", "error", "arity",
                     Cons::List(env, std::vector<Tag>{Symbol::Keyword("nreq"),
                                                      Fixnum(nreqs).tag_,
                                                      Symbol::Keyword("from"),
                                                      Cons::List(env, argv)}));

  Frame fp = Frame(func, const_cast<Tag *>(argv.data()));
  Tag fn = Function::form(env, func);

  if (Symbol::IsType(fn)) {
    ctx.DynamicContextPush(fp);
    env.fn_cache->at(fn)(ctx, fp);
    ctx.DynamicContextPop();

    return fp.value;
  }

  if (!Cons::IsType(fn))
    throw std::runtime_error("funcall type");

  ctx.DynamicContextPush(fp);

  if (nreqs)
    ctx.LexicalContextPush(Function::frame_id(env, func), fp);

  fp.value = Cons::cdr(env, fn);
  Cons::iter iter(env, fp.value);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    fp.value = Eval(ctx, Cons::car(env, it));

  if (nreqs)
    ctx.LexicalContextPop(Function::frame_id(env, func));

  ctx.DynamicContextPop();

  return fp.value;
}

/** * evaluate form in environment **/
Tag Env::Eval(Context &ctx, Tag form) {
  Env &env = ctx.env;

  if (Symbol::IsType(form)) {
    if (!Symbol::IsBound(env, form))
      Exception::Raise(env, "eval", "error", "unbound", form);

    return Symbol::value(env, form);
  }

  if (!Cons::IsType(form))
    return form;

  Tag fn = Cons::car(env, form);

  if (Symbol::IsType(fn)) {
    if (Type::Eq(fn, Symbol::Keyword("quote")))
      return Cons::Nth(env, form, 1);

    if (Type::Eq(fn, Symbol::Keyword("if")))
      return Funcall(ctx,
                     Type::Null(Eval(ctx, Cons::Nth(env, form, 1)))
                         ? Cons::Nth(env, form, 3)
                         : Cons::Nth(env, form, 2),
                     std::vector<Tag>{});

    if (!Symbol::IsBound(env, fn))
      Exception::Raise(env, "eval", "error", "unbound", fn);

    fn = Symbol::value(env, fn);
  } else if (Cons::IsType(form)) {
    fn = Eval(ctx, fn);
  }

  if (!Function::IsType(fn))
    Exception::Raise(env, "eval", "error", "type", fn);

  Tag args = Cons::cdr(env, form);

  std::vector<Tag> vlist{};
  if (!Type::Null(args)) {
    vlist.reserve(Cons::Length(env, args));

    Cons::iter iter(env, args);
    for (auto it = iter.begin(); it != iter.end(); it = ++iter)
      vlist.push_back(Eval(ctx, Cons::car(env, it)));
  }

  return Funcall(ctx, fn, vlist);
}

} /* namespace core */
} /* namespace libmu */
