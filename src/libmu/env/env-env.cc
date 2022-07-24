/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  env-env.cc: library env
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

/** * (ev-info) => alist **/
void EnvInfo(Context &ctx, Frame &fp) {
  Env &env = ctx.env;
  std::vector<Tag> info{};

  /** * version **/
  info.push_back(
      Cons(Symbol::Keyword("version"), Symbol::value(env, env.version))
          .Heap(env));

  /** * namespaces **/
  std::vector<Tag> namespaces{};

  for (auto [name, ns] : env.namespaces)
    namespaces.push_back(Cons(Vector(name).Heap(env), ns).Heap(env));

  info.push_back(
      Cons(Symbol::Keyword("ns"), Cons::List(env, namespaces)).Heap(env));

  /** * heaps and such **/
  fp.value = Cons::List(env, info);
}

/** * (hp-info type) => vector **/
void HeapInfo(Context &ctx, Frame &fp) {
  Tag heap = fp.argv[0];
  Tag type = fp.argv[1];

  if (!Symbol::IsKeyword(heap))
    Exception::Raise(ctx.env, "hp-info", "error", "type", heap);

  if (!Symbol::IsKeyword(type) || !Type::IsClassSymbol(type))
    Exception::Raise(ctx.env, "hp-info", "error", "type", type);

  Env &env = ctx.env;
  std::function<Type::Tag(int, Type::SYS_CLASS)> type_vec =
      [&env](int size, Type::SYS_CLASS sys_class) {
        return Vector(std::vector<Type::Tag>{
                          Fixnum(-1).tag_, /* figure out per object size */
                          Fixnum(size).tag_,
                          Fixnum(env.heap->TypeAlloc(sys_class)).tag_})
            .Heap(env);
      };

  /** * directs return :nil */
  Type::SYS_CLASS sys_class = Type::MapSymbolClass(type);

  switch (sys_class) {
  case Type::SYS_CLASS::BYTE:
  case Type::SYS_CLASS::FIXNUM:
    [[fallthrough]];
  case Type::SYS_CLASS::FLOAT:
    fp.value = Type::NIL;
    break;
  case Type::SYS_CLASS::T:
    fp.value =
        Vector(std::vector<Type::Tag>{Fixnum(ctx.env.heap->HeapSize()).tag_,
                                      Fixnum(ctx.env.heap->HeapAlloc()).tag_,
                                      Fixnum(ctx.env.heap->TypeAlloc()).tag_})
            .Heap(ctx.env);
    break;
  default:
    fp.value = type_vec(ctx.env.heap->Room(sys_class), sys_class);
    break;
  }
}

/** * (gc) => boolean **/
void Gc(Context &ctx, Frame &fp) {
  Env &env = ctx.env;

  env.heap->Gc(env);

  for (auto [name, ns] : env.namespaces)
    Env::Gc(env, ns);

  env.heap->GcSweep(env);
  fp.value = Fixnum(env.heap->nfree).tag_;
}

/** * (saveimg form env => object **/
void SaveImage(Context &ctx, Frame &fp) {
  Tag fn = fp.argv[0];

  if (!Vector::IsTyped(ctx.env, fn, Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, "sv-img", "error", "type", fn);

  system::System::SaveImage(ctx.env.sys, Vector::StdStringOf(ctx.env, fn));

  fp.value = Type::T;
}

} /* namespace env */
} /* namespace libmu */
