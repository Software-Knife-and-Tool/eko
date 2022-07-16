/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 ** mu-symbol.cc: library symbol functions
 **
 **/
#include <cassert>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/function.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

namespace libmu {

using Exception = type::Exception;
using Function = type::Function;
using Symbol = type::Symbol;
using Vector = type::Vector;

using Context = core::Context;
using Env = core::Env;
using Frame = core::Context::Frame;
using Tag = core::Tag;
using Type = core::Type;

namespace mu {

/** * (sy-val symbol) => object **/
void SymbolValue(Context &ctx, Frame &fp) {
  Tag symbol = fp.argv[0];

  if (!Symbol::IsType(ctx.env, symbol))
    Exception::Raise(ctx.env, "sy-val", "error", "type", symbol);

  if (!Symbol::IsBound(ctx.env, symbol))
    Exception::Raise(ctx.env, "sy-val", "error", "unbound", symbol);

  fp.value = Symbol::value(ctx.env, symbol);
}

/** * (sy-name symbol) => string **/
void SymbolName(Context &ctx, Frame &fp) {
  Tag symbol = fp.argv[0];

  if (!Symbol::IsType(ctx.env, symbol))
    Exception::Raise(ctx.env, "sy-name", "error", "type", symbol);

  fp.value = Symbol::name(ctx.env, symbol);
}

/** * (sy-ns symbol) => namespace **/
void SymbolNamespace(Context &ctx, Frame &fp) {
  Tag symbol = fp.argv[0];

  if (!Symbol::IsType(ctx.env, symbol))
    Exception::Raise(ctx.env, "sy-ns", "error", "type", symbol);

  fp.value = Symbol::ns(ctx.env, symbol);
}

/** * (keysymp object) => bool **/
void IsKeyword(Context &, Frame &fp) {
  fp.value = Type::Bool(Symbol::IsKeyword(fp.argv[0]));
}

/** * (boundp symbol) => bool **/
void IsBound(Context &ctx, Frame &fp) {
  fp.value = Type::Bool(Symbol::IsBound(ctx.env, fp.argv[0]));
}

/** * (symbol string) => symbol **/
void UninternedSymbol(Context &ctx, Frame &fp) {
  if (!Vector::IsTyped(ctx.env, fp.argv[0], Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, "unin-sy", "error", "type", fp.argv[0]);

  fp.value = Symbol(Type::NIL, fp.argv[0], Type::UNBOUND).Heap(ctx.env);
}

/** * (keyword string) */
void Keyword(Context &ctx, Frame &fp) {
  if (!Vector::IsTyped(ctx.env, fp.argv[0], Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, "key", "error", "type", fp.argv[0]);

  fp.value = Symbol::Keyword(fp.argv[0]);
}

} /* namespace mu */
} /* namespace libmu */
