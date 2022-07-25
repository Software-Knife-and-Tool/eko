/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 ** symbol.cc: library symbols
 **
 **/
#include "libmu/type/symbol.h"

#include <cassert>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/readtable.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/function.h"
#include "libmu/type/namespace.h"
#include "libmu/type/stream.h"
#include "libmu/type/vector.h"

namespace libmu {

using Heap = core::Heap;

namespace type {

using SCOPE = Namespace::SCOPE;

namespace {

/** * parse symbol namespace designator **/
Tag namespace_of(Env &env, const std::string &symbol, const std::string &sep) {
  size_t cpos = symbol.find(sep);

  if (cpos < symbol.size()) {
    std::optional<Tag> ns = env.MapNamespace(symbol.substr(0, cpos));
    if (!ns.has_value())
      Exception::Raise(env, "sy-ns", "error", "ns",
                       Vector(symbol.substr(0, cpos)).Heap(env));
    return ns.value();
  }

  return Type::NIL;
}

/** * parse symbol name string **/
Tag name_of(Env &env, const std::string &symbol, const std::string &sep) {
  size_t cpos = symbol.find(sep);

  return Vector((cpos < symbol.size()) ? symbol.substr(cpos + sep.length())
                                       : symbol)
      .Heap(env);
}

} /* anonymous namespace */

bool Symbol::IsType(Tag ptr) {
  return IsKeyword(ptr) ||
         (IsIndirect(ptr) && IndirectClass(ptr) == SYS_CLASS::SYMBOL);
}

Tag Symbol::ns(Env &env, Tag symbol) {
  if (IsKeyword(symbol))
    return NIL;

  auto ns = Heap::Layout<Layout>(env, symbol)->ns;
  return ns;
  //  return Null(ns) ? env.uninterned : ns;
}

/** * garbage collection **/
void Symbol::Gc(Env &env, Tag ptr) {
  assert(IsType(ptr));

  if (IsKeyword(ptr) || Env::IsGcMarked(env, ptr))
    return;

  Env::GcMark(env, ptr);

  assert(!IsKeyword(ptr));

  Env::Gc(env, name(env, ptr));
  Env::Gc(env, value(env, ptr));
}

/** * keywords **/
Tag Symbol::Keyword(Tag name) {
  return MakeDirect(DirectData(name), DirectSize(name), DIRECT_CLASS::SYMBOL);
}

Tag Symbol::Keyword(const std::string &name) {
  return Keyword(Vector::MakeDirect(name));
}

/** * is symbol bound to a value? */
bool Symbol::IsBound(Env &env, Tag sym) {
  assert(IsType(sym));

  return IsKeyword(sym) || !Eq(value(env, sym), UNBOUND);
}

/** * is symbol not interned? */
bool Symbol::IsUninterned(Env &env, Tag sym) {
  assert(IsType(sym));

  if (IsKeyword(sym))
    return false;

  return Null(ns(env, sym)) || Eq(ns(env, sym), env.uninterned);
}

/** * print symbol to stream **/
void Symbol::Write(Env &env, Tag sym, Tag stream, bool esc) {
  assert(IsType(sym));
  assert(Stream::IsType(stream));

  if (IsKeyword(sym)) {
    Env::Write(env, ":", stream, false);
  } else if (esc) {
    Tag ns = Symbol::ns(env, sym);

    if (Null(ns)) {
      Env::Write(env, "#:", stream, false);
    } else {
      Env::Write(env, Vector::StdStringOf(env, Namespace::name(env, ns)),
                 stream, false);
      if (Namespace::Map(env, ns, SCOPE::EXTERN, Symbol::name(env, sym))
              .has_value())
        Env::Write(env, ":", stream, false);
      else if (Namespace::Map(env, ns, SCOPE::INTERN, Symbol::name(env, sym))
                   .has_value())
        Env::Write(env, "::", stream, false);
      else
        assert(false);
    }
  }

  Env::Write(env, Vector::StdStringOf(env, Symbol::name(env, sym)), stream,
             false);
}

/** * parse symbol **/
Tag Symbol::ParseSymbol(Env &env, std::string string) {

  if (string.size() == 0)
    Exception::Raise(env, "sy-pars", "error", "size", Type::NIL);

  if (string.size() == 1 && string[0] == '.')
    return Tag(core::ReadTable::SYNTAX_CHAR::DOT);

  if (string[0] == ':') {
    if (string.size() == 1)
      Exception::Raise(env, "sy-pars", "error", "eof", Type::NIL);

    if (string.size() - 1 > Type::DIRECT_STR_MAX)
      Exception::Raise(env, "sy-pars", "error", "size",
                       Vector(string).Heap(env));

    std::string key = string;
    return Keyword(key.erase(0, 1));
  }

  Tag int_ns = namespace_of(env, string, "::");
  if (!Null(int_ns))
    return Namespace::Intern(env, int_ns, SCOPE::INTERN,
                             name_of(env, string, "::"), Type::UNBOUND);

  Tag ext_ns = namespace_of(env, string, ":");
  if (!Null(ext_ns))
    return Namespace::Intern(env, ext_ns, SCOPE::EXTERN,
                             name_of(env, string, ":"), Type::UNBOUND);

  return Namespace::Intern(env, NIL, SCOPE::EXTERN, Vector(string).Heap(env),
                           Type::UNBOUND);
}

/** evict symbol to heap0 **/
Tag Symbol::Heap(Env &env) {
  auto alloc = env.heap->Alloc(sizeof(Layout), SYS_CLASS::SYMBOL);
  if (!alloc.has_value())
    throw std::runtime_error("heap exhausted");

  Tag tag = Entag(alloc.value(), SYS_CLASS::SYMBOL, TAG::INDIRECT);
  *env.heap->Layout<Layout>(env, tag) = symbol_;

  return tag;
}

Symbol::Symbol(Tag ns, Tag name, Tag value) {
  symbol_.ns = ns;
  symbol_.name = name;
  symbol_.value = value;
}

} /* namespace type */
} /* namespace libmu */
