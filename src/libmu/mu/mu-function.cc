/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu-function.cc: library functions
 **
 **/
#include <cassert>
#include <memory>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

namespace libmu {

using Cons = type::Cons;
using Exception = type::Exception;
using Fixnum = type::Fixnum;
using Function = type::Function;
using Symbol = type::Symbol;
using Vector = type::Vector;

using Context = core::Context;
using Frame = core::Context::Frame;
using Type = core::Type;

namespace mu {

/** * (funcall func list) => object **/
void Funcall(Context &ctx, Frame &fp) {
  Tag func = fp.argv[0];
  Tag args = fp.argv[1];

  if (!(Function::IsType(ctx.env, func)))
    Exception::Raise(ctx.env, "funcall", "error", "type", func);

  if (!(Cons::IsList(ctx.env, args)))
    Exception::Raise(ctx.env, "funcall", "error", "type", args);

  std::vector<Type::Tag> argv;
  Cons::iter iter(ctx.env, args);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    argv.push_back(Cons::car(ctx.env, it));

  fp.value = Env::Funcall(ctx, func, argv);
}

/** * (fclone vector) => clone function **/
void FunctionClone(Context &ctx, Frame &fp) {
  Tag vector = fp.argv[0];

  if (!Vector::IsType(ctx.env, vector))
    Exception::Raise(ctx.env, "clone", "error", "type", vector);

  fp.value = Function::Clone(ctx.env, vector);
}

/** * (fn-int fn keyword) => function internals **/
void FunctionInt(Context &ctx, Frame &fp) {
  Tag keyword = fp.argv[0];
  Tag fn = fp.argv[1];

  static const std::map<Type::Tag, std::function<Type::Tag(Env &, Type::Tag)>>
      kFuncAccessorMap{{Symbol::Keyword("arity"), Function::arity},
                       {Symbol::Keyword("ext"), Function::extension},
                       {Symbol::Keyword("form"), Function::form},
                       {Symbol::Keyword("frame"), Function::frame_id}};

  if (!Function::IsType(ctx.env, fn))
    Exception::Raise(ctx.env, "fn-int", "error", "type", fn);

  if (!Symbol::IsKeyword(keyword))
    Exception::Raise(ctx.env, "fn-int", "error", "type", keyword);

  if (!kFuncAccessorMap.contains(keyword))
    Exception::Raise(ctx.env, "fn-int", "error", "value", keyword);

  fp.value = kFuncAccessorMap.at(keyword)(ctx.env, fn);
}

} /* namespace mu */
} /* namespace libmu */
