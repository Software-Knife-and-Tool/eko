/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  compile.cc: library form
 **
 **/
#include "libmu/core/compile.h"

#include <cassert>
#include <functional>
#include <map>
#include <optional>
#include <utility>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
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

namespace core {
namespace {

/** * compile a list of forms **/
Tag compile_list(Context &ctx, Tag list) {
  assert(Cons::IsList(list));

  std::vector<Tag> vlist{};

  Cons::iter iter(ctx.env, list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    vlist.push_back(Compile::Form(ctx, Cons::car(ctx.env, it)));

  return Cons::List(ctx.env, vlist);
}

/** * is this symbol in the lexical environment? **/
std::optional<std::pair<Tag, int>> map_lexical_symbol(Env &env, Tag sym) {
  assert(Symbol::IsType(sym) || Symbol::IsKeyword(sym));

  if (Symbol::IsKeyword(sym))
    return std::nullopt;

  std::vector<Tag>::reverse_iterator it;
  for (it = env.compiler->lexicals.rbegin();
       it != env.compiler->lexicals.rend(); ++it) {
    Tag lexicals = Cons::car(env, Function::form(env, *it));

    Cons::iter iter(env, lexicals);
    int i = 0;
    for (auto lit = iter.begin(); lit != iter.end(); ++i, lit = ++iter)
      if (Type::Eq(sym, Cons::car(env, lit)))
        return std::pair<Tag, int>{*it, i};
  }

  return std::nullopt;
}

/** * parse lambda definition **/
void parse_lambda(Env &env, Tag lambda) {
  assert(Cons::IsList(lambda));

  std::vector<Tag> lexicals{};

  std::function<void(Env &, Tag)> parse = [&lexicals](Env &env, Tag symbol) {
    if (!Symbol::IsType(symbol))
      Exception::Raise(env, ":lambda", "error", "type", symbol);

    if (Symbol::IsKeyword(symbol))
      Exception::Raise(env, ":lambda", "error", "type", symbol);

#if 0
    if (!Symbol::IsUninterned(env, symbol))
      Exception::Raise(env, ":lambda", "error", "type", symbol);
#endif

    auto el = std::find_if(lexicals.begin(), lexicals.end(),
                           [symbol](Tag sym) { return Type::Eq(symbol, sym); });

    if (el != lexicals.end())
      Exception::Raise(env, ":lambda", "error", "dup", symbol);

    lexicals.push_back(symbol);
  };

  Cons::iter iter(env, lambda);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    parse(env, Cons::car(env, it));
}

/** * lambda special form **/
Tag def_lambda(Context &ctx, Tag form) {
  Env &env = ctx.env;

  if (Cons::Length(env, form) == 1)
    Exception::Raise(env, ":lambda", "error", "arity", form);

  Tag lambda = Cons::Nth(env, form, 1);
  Tag body = Cons::cdr(env, Cons::cdr(env, form));

  if (!Cons::IsList(lambda))
    Exception::Raise(env, ":lambda", "error", "type", lambda);

  if (Type::Eq(Cons::car(env, lambda), Symbol::Keyword("quote")))
    Exception::Raise(env, ":lambda", "error", "quote", lambda);

  parse_lambda(env, lambda);

  Tag fn = Function(env, env.compiler->frameid(), lambda,
                    Cons(lambda, Type::NIL).Heap(env))
               .Heap(env);

  if (Fixnum::Int64Of(Function::arity(env, fn)))
    env.compiler->lexicals.push_back(fn);

  Function::set_form(env, fn, Cons(lambda, compile_list(ctx, body)).Heap(env));

  if (Fixnum::Int64Of(Function::arity(env, fn)))
    env.compiler->lexicals.pop_back();

  return fn;
}

/** * quote special form **/
Tag def_quote(Context &ctx, Tag form) {
  Env &env = ctx.env;

  if (Cons::Length(env, form) != 2)
    Exception::Raise(env, ":quote", "error", "arity", form);

  return form;
}

/** * if special form **/
Tag def_conditional(Context &ctx, Tag form) {
  Env &env = ctx.env;

  if (Cons::Length(env, form) != 4)
    Exception::Raise(env, ":if", "error", "arity", form);

  return Cons::List(
      env,
      std::vector<Tag>{
          Symbol::Keyword("if"), Compile::Form(ctx, Cons::Nth(env, form, 1)),
          def_lambda(
              ctx, Cons::List(env, std::vector<Tag>{Symbol::Keyword("lambda"),
                                                    Type::NIL,
                                                    Cons::Nth(env, form, 2)})),
          def_lambda(ctx,
                     Cons::List(env, std::vector<Tag>{
                                         Symbol::Keyword("lambda"), Type::NIL,
                                         Cons::Nth(env, form, 3)}))});
}

/** * map special form keyword to handler **/
static const std::map<Tag, std::function<Tag(Context &, Tag)>> kSpecMap{
    {Symbol::Keyword("lambda"), def_lambda},
    {Symbol::Keyword("quote"), def_quote},
    {Symbol::Keyword("if"), def_conditional}};

} /* anonymous namespace */

/** * function frame id **/
Tag Compile::frameid() { return Symbol::Keyword(std::to_string(frame_id++)); }

/** * compile form **/
Tag Compile::Form(Context &ctx, Tag form) {
  Env &env = ctx.env;

  switch (Type::TypeOf(form)) {
  case SYS_CLASS::CONS: { /* funcall/special call */
    Tag fn = Cons::car(env, form);

    switch (Type::TypeOf(fn)) {
    case SYS_CLASS::CONS: { /* list, should compile to a function */
      Tag func = Form(ctx, fn);

      if (!Function::IsType(func))
        Exception::Raise(env, "compile", "error", "type", fn);

      return Cons(func, compile_list(ctx, Cons::cdr(env, form))).Heap(env);
    }
    case SYS_CLASS::SYMBOL: { /* funcall/special call */
      auto lex_env = map_lexical_symbol(env, fn);

      if (Symbol::IsKeyword(fn) && !kSpecMap.contains(fn))
        Exception::Raise(env, "compile", "error", "type", fn);

      return lex_env.has_value()     ? compile_list(ctx, form)
             : Symbol::IsKeyword(fn) ? kSpecMap.at(fn)(ctx, form)
                                     : compile_list(ctx, form);
    }
    case SYS_CLASS::FUNCTION: /* function */
      return compile_list(ctx, form);
    default:
      Exception::Raise(env, "compile", "error", "type", fn);
    }
  }
  case SYS_CLASS::SYMBOL: {
    if (Symbol::IsKeyword(form))
      return form;

    std::optional<std::pair<Tag, int>> lex_env = map_lexical_symbol(env, form);

    if (lex_env.has_value()) {
      const auto [fn, offset] = lex_env.value();

      return Form(
          ctx, Cons::List(env, std::vector<Tag>{
                                   Namespace::Map(env, env.env, SCOPE::EXTERN,
                                                  Vector("lex-ref").Heap(env))
                                       .value(),
                                   Function::frame_id(env, fn),
                                   Fixnum(offset).tag_}));
    }
    break;
  }
  default: /* constant */
    break;
  }

  /* non-lexical symbols and constants */
  return form;
}

} /* namespace core */
} /* namespace libmu */
