/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 ** namespace.cc: library namespaces
 **
 **/
#include "libmu/type/namespace.h"

#include <cassert>
#include <vector>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/function.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

namespace libmu {

using Heap = core::Heap;

namespace type {
namespace {

Tag hash_id(Env &env, Tag str) {
  static const int FNV_prime = 16777619;
  static const uint64_t OFFSET_BASIS = 2166136261UL;

  assert(Vector::IsTyped(env, str, Type::SYS_CLASS::CHAR));

  int64_t hash = OFFSET_BASIS;

  Vector::iter<char> iter(env, str);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    hash ^= *it;
    hash *= FNV_prime;
  }

  return Fixnum(hash).tag_;
}

} /* anonymous namespace */

bool Namespace::IsType(Env &env, Tag ptr) {
  return IsIndirect(ptr) &&
         env.heap->SysClass(env, ptr) == SYS_CLASS::NAMESPACE;
}

/** * resolve namespace designator **/
Tag Namespace::NsDesignator(Env &env, Tag ns) {
  assert(IsType(env, ns) || Null(ns));

  return Null(ns) ? env.uninterned : ns;
}

/** * find symbol in namespace externs **/
std::optional<Tag> Namespace::Map(Env &env, Tag ns, SCOPE scope, Tag name) {
  assert(IsType(env, ns));
  assert(Vector::IsTyped(env, name, SYS_CLASS::CHAR));

  switch (scope) {
  case SCOPE::EXTERN: {
    std::optional<Tag> sym =
        env.ns_cache->find_in_externs(ns, hash_id(env, name));

    if (sym.has_value()) {
      std::string sym_name =
          Vector::StdStringOf(env, Symbol::name(env, sym.value()));

      if (sym_name.compare(Vector::StdStringOf(env, name)))
        throw std::runtime_error("hash collision");
    }

    return sym;
  }
  case SCOPE::INTERN: {
    std::optional<Tag> sym =
        env.ns_cache->find_in_interns(ns, hash_id(env, name));

    if (sym.has_value()) {
      std::string sym_name =
          Vector::StdStringOf(env, Symbol::name(env, sym.value()));

      if (sym_name.compare(Vector::StdStringOf(env, name)))
        throw std::runtime_error("hash collision");
    }

    return sym;
  }
  }
}

/** * garbage collect the namespace **/
void Namespace::Gc(Env &env, Tag ptr) {
  assert(IsType(env, ptr));

  if (Env::IsGcMarked(env, ptr))
    return;

  Env::GcMark(env, ptr);

  Env::Gc(env, name(env, ptr));
  Env::Gc(env, import(env, ptr));
  Env::Gc(env, externs(env, ptr));
  Env::Gc(env, interns(env, ptr));
}

/** * view of namespace object **/
Tag Namespace::View(Env &env, Tag ns) {
  assert(IsType(env, ns));

  std::vector<Tag> view =
      std::vector<Tag>{name(env, ns), Fixnum(0).tag_, Fixnum(1).tag_};

  return Vector(view).Heap(env);
}

/** * intern symbol in namespace **/
Tag Namespace::Intern(Env &env, Tag nsd, SCOPE scope, Tag name, Tag value) {
  assert(IsType(env, nsd) || Null(nsd));
  assert(Vector::IsTyped(env, name, SYS_CLASS::CHAR));

  Tag ns = NsDesignator(env, nsd);
  std::optional<Tag> nf = Map(env, ns, scope, name);

  if (!nf.has_value()) {
    Tag sym = Symbol(nsd, name, value).Heap(env);

    switch (scope) {
    case SCOPE::EXTERN:
      externs(env, ns, Cons(sym, externs(env, ns)).Heap(env));
      env.ns_cache->add_externs(ns, hash_id(env, name), sym);
      break;
    case SCOPE::INTERN:
      interns(env, ns, Cons(sym, interns(env, ns)).Heap(env));
      env.ns_cache->add_interns(ns, hash_id(env, name), sym);
      break;
    }

    return sym;
  }

  if (!Symbol::IsBound(env, nf.value()))
    Symbol::set_value(env, nf.value(), value);

  return nf.value();
}

/** * print namespace **/
void Namespace::Write(Env &env, Tag nsd, Tag stream, bool) {
  assert(IsType(env, nsd) || Null(nsd));
  assert(Stream::IsType(env, stream));

  Tag ns = NsDesignator(env, nsd);
  std::string type = Vector::StdStringOf(
      env, Symbol::name(env, Type::MapClassSymbol(Type::TypeOf(env, ns))));

  std::string name = Null(Namespace::name(env, ns))
                         ? "uninterned"
                         : Vector::StdStringOf(env, Namespace::name(env, ns));

  std::stringstream hexs;

  hexs << std::hex << std::to_underlying(ns);
  core::Env::Write(env,
                   "#<:" + type + " #x" + hexs.str() + " (\"" + name + "\")>",
                   stream, false);
}

Tag Namespace::Heap(Env &env) {
  auto alloc = env.heap->Alloc(sizeof(Layout), SYS_CLASS::NAMESPACE);
  if (!alloc.has_value())
    throw std::runtime_error("heap exhausted");

  Tag tag = Entag(alloc.value(), TAG::INDIRECT);
  *env.heap->Layout<Layout>(env, tag) = namespace_;

  return tag;
}

/** * namespace constructor **/
Namespace::Namespace(Tag name, Tag import) : Type() {
  namespace_.name = name;
  namespace_.import = import;
  namespace_.externs = NIL;
  namespace_.interns = NIL;
}

} /* namespace type */
} /* namespace libmu */
