/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  env.cc: library environment
 **
 **/
#include "libmu/core/env.h"

#include <cassert>
#include <functional>
#include <map>
#include <mutex>
#include <tuple>
#include <vector>

#include "libmu/core/compile.h"
#include "libmu/core/heap.h"
#include "libmu/core/nscache.h"
#include "libmu/core/type.h"

#include "libmu/heap/mapped.h"

#include "libmu/env/env.h"
#include "libmu/mu/mu.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/namespace.h"
#include "libmu/type/stream.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

#include "libmu/system/internals.h"
#include "libmu/system/system.h"

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

using SCOPE = Namespace::SCOPE;

namespace core {
namespace {

/** * mu extern functions **/
static const std::vector<Env::MuFunctionDef> kMuExtFuncTab{
    {"boundp", mu::IsBound, 1},
    {"car", mu::Car, 1},
    {"ch-code", mu::CharCode, 1},
    {"cdr", mu::Cdr, 1},
    {"close", mu::Close, 1},
    {"code-ch", mu::CodeChar, 1},
    {"cons", mu::Cons, 2},
    {"eofp", mu::IsEof, 1},
    {"eq", mu::Eq, 2},
    {"eval", mu::Eval, 1},
    {"except", mu::Exception, 4},
    {"fixnum*", mu::FixMul, 2},
    {"fixnum+", mu::FixAdd, 2},
    {"fixnum-", mu::FixSub, 2},
    {"fixnum<", mu::FixLessThan, 2},
    {"float*", mu::FloatMul, 2},
    {"float+", mu::FloatAdd, 2},
    {"float-", mu::FloatSub, 2},
    {"float/", mu::FloatDiv, 2},
    {"float<", mu::FloatLessThan, 2},
    {"floor", mu::Floor, 2},
    {"funcall", mu::Funcall, 2},
    {"get-str", mu::GetStringStream, 1},
    {"intern", mu::Intern, 3},
    {"keyword", mu::Keyword, 1},
    {"keysymp", mu::IsKeyword, 1},
    {"length", mu::ListLength, 1},
    {"list-sv", mu::VectorCons, 2},
    {"logand", mu::Logand, 2},
    {"logor", mu::Logor, 2},
    {"ns", mu::MakeNamespace, 2},
    {"ns-find", mu::MapNamespace, 3},
    {"ns-imp", mu::ImportOfNamespace, 1},
    {"ns-name", mu::NameOfNamespace, 1},
    {"ns-syms", mu::NamespaceSymbols, 2},
    {"open", mu::OpenStream, 3},
    {"nth", mu::Nth, 2},
    {"nthcdr", mu::Nthcdr, 2},
    {"raise", mu::RaiseException, 1},
    {"rd-byte", mu::ReadByte, 1},
    {"rd-char", mu::ReadChar, 1},
    {"read", mu::Read, 1},
    {"slice", mu::VectorSlice, 3},
    {"sv-len", mu::VectorLength, 1},
    {"sv-ref", mu::VectorRef, 2},
    {"sv-type", mu::VectorType, 1},
    {"sy-name", mu::SymbolName, 1},
    {"sy-ns", mu::SymbolNamespace, 1},
    {"sy-val", mu::SymbolValue, 1},
    {"symbol", mu::UninternedSymbol, 1},
    {"trunc", mu::Truncate, 2},
    {"type-of", mu::TypeOf, 1},
    {"un-byte", mu::UnReadByte, 2},
    {"un-char", mu::UnReadChar, 2},
    {"with-ex", mu::WithException, 2},
    {"wr-byte", mu::WriteByte, 2},
    {"wr-char", mu::WriteChar, 2},
    {"write", mu::Write, 3}};

/** * mu intern functions **/
static const std::vector<Env::MuFunctionDef> kMuIntFuncTab{
    {"compile", mu::Compile, 1},
    {"fclone", mu::FunctionClone, 1},
    {"fn-int", mu::FunctionInt, 2},
    {"intern", mu::InternValue, 4},
    {"view", mu::View, 1}};

/** * env extern functions **/
static const std::vector<Env::MuFunctionDef> kEnvExtFuncTab{
    {"bind", env::BindContext, 2},
    {"context", env::GetContext, 0},
    {"cx-info", env::ContextInfo, 0},
    {"ev-info", env::EnvInfo, 0},
    {"find-ns", env::MapNamespace, 1},
    {"fix", env::ContextFix, 1},
    {"fnv-lex", env::FrameGet, 1},
    {"fnv-set", env::FrameSetVar, 3},
    {"gc", env::Gc, 0},
    {"hp-info", env::HeapInfo, 2},
    {"lex-pop", env::FramePop, 1},
    {"lex-psh", env::FramePush, 1},
    {"lex-ref", env::FrameRef, 2},
    {"lex-set", env::FrameSet, 3},
    {"ns", env::Namespace, 1},
    {"resume", env::ResumeContext, 1},
    {"saveimg", env::SaveImage, 1},
    {"suspend", env::SuspendContext, 2},
};

/** * env intern functions **/
static const std::vector<Env::MuFunctionDef> kEnvIntFuncTab{};

/** * system extern functions **/
static const std::vector<Env::MuFunctionDef> kSystemExtFuncTab{
    {"cwd", system::Cwd, 0},
    {"chdir", system::Chdir, 1},
    {"errno", system::Errno, 0},
    {"exit", system::Exit, 1},
    {"logname", system::Logname, 0},
    {"env", system::ProcessEnv, 0},
    {"system", system::SystemCommand, 1},
    {"utimes", system::Times, 0}};

/** * system intern functions **/
static const std::vector<Env::MuFunctionDef> kSystemIntFuncTab{};

/** * bind native functions **/
void bind_functions(Env &env, Tag ns, SCOPE scope,
                    const std::vector<Env::MuFunctionDef> &fns) {

  for (auto &el : fns) {
    assert(strlen(el.name) <= 7);

    Tag tag = Symbol(ns, Vector(el.name).Heap(env), Type::NIL).Heap(env);

    Namespace::Intern(env, ns, scope, Symbol::name(env, tag),
                      Function(tag, &el).Heap(env));
    env.fn_cache->insert({tag, el.fn});
  }
}

} /* anonymous namespace */

/** look up namespace by name in environment map **/
std::optional<Tag> Env::MapNamespace(const std::string &ns) const {
  auto ns_iter = namespaces.find(ns);

  if (ns_iter == namespaces.end())
    return std::nullopt;

  return ns_iter->second;
}

/** * add namespace object to environment map **/
void Env::AddNamespace(Tag ns) {
  assert(Namespace::IsType(ns));

  auto ns_name = Vector::StdStringOf(*this, Namespace::name(*this, ns));
  auto ns_iter = namespaces.find(ns_name);

  if (ns_iter == namespaces.end()) {
    namespaces[ns_name] = ns;
    ns_cache->add_ns_cache(ns);
  }
}

/** * garbage collection **/
void Env::Gc(Env &env, Tag ptr) {

  std::function<void(Env &, Tag)> noGc = [](Env &, Tag) {};
  static const std::map<SYS_CLASS, std::function<void(Env &, Tag)>> kGcTypeMap{
      {SYS_CLASS::CHAR, noGc},
      {SYS_CLASS::CONS, type::Cons::Gc},
      {SYS_CLASS::DOUBLE, noGc},
      {SYS_CLASS::EXCEPTION, type::Exception::Gc},
      {SYS_CLASS::FIXNUM, noGc},
      {SYS_CLASS::FLOAT, noGc},
      {SYS_CLASS::FUNCTION, type::Function::Gc},
      {SYS_CLASS::NAMESPACE, type::Namespace::Gc},
      {SYS_CLASS::STREAM, noGc},
      {SYS_CLASS::SYMBOL, type::Symbol::Gc},
      {SYS_CLASS::VECTOR, type::Vector::Gc}};

  assert(kGcTypeMap.contains(Type::TypeOf(ptr)));
  kGcTypeMap.at(Type::TypeOf(ptr))(env, ptr);
}

/** * env constructor **/
Env::Env(system::System &env_sys) : sys(env_sys) {
  Env &_env = *this;

  // need to get number of pages from command line or process env;
  // -= runtime configuration needs thought =-
  heap = std::make_unique<heap::Mapped>(env_sys, 1024);

  readtable = std::make_unique<ReadTable>();
  compiler = std::make_unique<Compile>();

  uninterned = Namespace(Vector("").Heap(_env), Type::NIL).Heap(_env);

  system = Namespace(Vector("system").Heap(_env), Type::NIL).Heap(_env);
  env = Namespace(Vector("env").Heap(_env), system).Heap(_env);
  mu = Namespace(Vector("mu").Heap(_env), env).Heap(_env);

  namespaces["env"] = env;
  namespaces["mu"] = mu;
  namespaces["system"] = system;

  ns_cache = std::make_unique<NSCache>();

  ns_cache->add_ns_cache(env);
  ns_cache->add_ns_cache(mu);
  ns_cache->add_ns_cache(system);
  ns_cache->add_ns_cache(uninterned);

  version =
      Namespace::Intern(_env, mu, SCOPE::EXTERN, Vector("version").Heap(_env),
                        Vector(VERSION).Heap(_env));

  standard_input =
      Namespace::Intern(_env, mu, SCOPE::EXTERN, Vector("std-in").Heap(_env),
                        Stream(sys.stdin).Heap(_env));

  standard_output =
      Namespace::Intern(_env, mu, SCOPE::EXTERN, Vector("std-out").Heap(_env),
                        Stream(sys.stdout).Heap(_env));

  error_output =
      Namespace::Intern(_env, mu, SCOPE::EXTERN, Vector("err-out").Heap(_env),
                        Stream(sys.stderr).Heap(_env));

  fn_cache = std::make_unique<FnCache>();

  bind_functions(_env, env, SCOPE::EXTERN, kEnvExtFuncTab);
  bind_functions(_env, env, SCOPE::INTERN, kEnvIntFuncTab);
  bind_functions(_env, mu, SCOPE::EXTERN, kMuExtFuncTab);
  bind_functions(_env, mu, SCOPE::INTERN, kMuIntFuncTab);
  bind_functions(_env, system, SCOPE::EXTERN, kSystemExtFuncTab);
  bind_functions(_env, system, SCOPE::INTERN, kSystemIntFuncTab);
}

} /* namespace core */
} /* namespace libmu */
