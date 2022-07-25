/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 ** mu-namespace.cc: library namespace functions
 **
 **/
#include <cassert>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/function.h"
#include "libmu/type/namespace.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

namespace libmu {

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
using Tag = core::Tag;
using Type = core::Type;

namespace mu {

/** * (ns-name namespace) => string **/
void NameOfNamespace(Context &ctx, Frame &fp) {
  Tag ns = Namespace::NsDesignator(ctx.env, fp.argv[0]);

  if (!Namespace::IsType(ns))
    Exception::Raise(ctx.env, "name-ns", "error", "type", ns);

  fp.value = Namespace::name(ctx.env, ns);
}

/** * (ns-imp namespace) => ns **/
void ImportOfNamespace(Context &ctx, Frame &fp) {
  Tag ns = Namespace::NsDesignator(ctx.env, fp.argv[0]);

  if (!Namespace::IsType(ns))
    Exception::Raise(ctx.env, "name-ns", "error", "type", ns);

  fp.value = Namespace::import(ctx.env, ns);
}

/** * (ns-syms namespace keyword) => list **/
void NamespaceSymbols(Context &ctx, Frame &fp) {
  Tag ns = Namespace::NsDesignator(ctx.env, fp.argv[0]);
  Tag type = fp.argv[1];

  if (!Namespace::IsType(ns))
    Exception::Raise(ctx.env, "ns-syms", "error", "type", ns);

  if (!Symbol::IsType(type))
    Exception::Raise(ctx.env, "ns-syms", "error", "type", type);

  std::vector<Type::Tag> symbols{};

  if (Type::Eq(type, Symbol::Keyword("intern"))) {
    fp.value = Namespace::interns(ctx.env, ns);
  } else if (Type::Eq(type, Symbol::Keyword("extern"))) {
    fp.value = Namespace::externs(ctx.env, ns);
  } else
    Exception::Raise(ctx.env, "ns-syms", "error", "value", type);
}

/** * (ns-find ns :intern|:extern string) => symbol **/
void MapNamespace(Context &ctx, Frame &fp) {
  Tag ns = Namespace::NsDesignator(ctx.env, fp.argv[0]);
  Tag type = fp.argv[1];
  Tag name = fp.argv[2];

  if (!Namespace::IsType(ns))
    Exception::Raise(ctx.env, "ns-find", "error", "type", ns);

  if (!Vector::IsTyped(ctx.env, name, Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, "ns-find", "error", "type", name);

  if (Type::Eq(Symbol::Keyword("intern"), type)) {
    std::optional<Tag> symbol =
        Namespace::Map(ctx.env, ns, SCOPE::INTERN, name);
    fp.value = symbol.has_value() ? symbol.value() : Type::NIL;
  } else if (Type::Eq(Symbol::Keyword("extern"), type)) {
    std::optional<Tag> symbol =
        Namespace::Map(ctx.env, ns, SCOPE::EXTERN, name);
    fp.value = symbol.has_value() ? symbol.value() : Type::NIL;
  } else
    Exception::Raise(ctx.env, "ns-find", "error", "type", name);
}

/** * (intern ns keyword symbol) => symbol **/
void Intern(Context &ctx, Frame &fp) {
  Tag ns = fp.argv[0];
  Tag type = fp.argv[1];
  Tag name = fp.argv[2];

  if (!Namespace::IsType(Namespace::NsDesignator(ctx.env, ns)))
    Exception::Raise(ctx.env, "intern", "error", "type", ns);

  if (!Vector::IsTyped(ctx.env, name, Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, "intern", "error", "type", name);

  if (Type::Eq(Symbol::Keyword("intern"), type)) {
    fp.value =
        Namespace::Intern(ctx.env, ns, SCOPE::INTERN, name, Type::UNBOUND);
  } else if (Type::Eq(Symbol::Keyword("extern"), type)) {
    fp.value =
        Namespace::Intern(ctx.env, ns, SCOPE::EXTERN, name, Type::UNBOUND);
  } else
    Exception::Raise(ctx.env, "intern", "error", "type", name);
}

/** * (:intern ns keyword symbol value) => symbol **/
void InternValue(Context &ctx, Frame &fp) {
  Tag ns = fp.argv[0];
  Tag type = fp.argv[1];
  Tag name = fp.argv[2];
  Tag value = fp.argv[3];

  if (!Namespace::IsType(Namespace::NsDesignator(ctx.env, ns)))
    Exception::Raise(ctx.env, ":intern", "error", "type", ns);

  if (!Vector::IsTyped(ctx.env, name, Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, ":intern", "error", "type", name);

  if (Type::Eq(Symbol::Keyword("intern"), type)) {
    fp.value = Namespace::Intern(ctx.env, ns, SCOPE::INTERN, name, value);
  } else if (Type::Eq(Symbol::Keyword("extern"), type)) {
    fp.value = Namespace::Intern(ctx.env, ns, SCOPE::EXTERN, name, value);
  } else
    Exception::Raise(ctx.env, "intern", "error", "type", name);
}

/** * (ns string import) => namespace **/
void MakeNamespace(Context &ctx, Frame &fp) {
  Tag name = fp.argv[0];
  Tag import = fp.argv[1];

  if (!Vector::IsTyped(ctx.env, name, Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, "ns", "error", "type", name);

  if (!Namespace::IsType(import) && !Type::Null(import))
    Exception::Raise(ctx.env, "ns", "error", "type", import);

  fp.value = type::Namespace(name, import).Heap(ctx.env);
}

} /* namespace mu */
} /* namespace libmu */
