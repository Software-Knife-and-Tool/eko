/********
 **
 **  SPDX-License-Identifier: MIT
 **
 **  Copyright (c) 2017-2022 James M. Putnam <putnamjm.design@gmail.com>
 **
 **/

/********
 **
 **  function.cc: library functions
 **
 **/
#include "libmu/type/function.h"

#include <cassert>
#include <sstream>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

namespace libmu {
namespace type {

using SCOPE = Namespace::SCOPE;

bool Function::IsType(Env &env, Tag ptr) {
  return Type::IsIndirect(ptr) &&
         env.heap->SysClass(env, ptr) == SYS_CLASS::FUNCTION;
}

/** * garbage collection **/
void Function::Gc(Env &env, Tag ptr) {
  assert(IsType(env, ptr));

  if (Env::IsGcMarked(env, ptr))
    return;

  Env::GcMark(env, ptr);

  Env::Gc(env, form(env, ptr));
  Env::Gc(env, extension(env, ptr));
}

/** * function printer **/
void Function::Write(Env &env, Tag fn, Tag stream, bool) {
  assert(IsType(env, fn));
  assert(Stream::IsType(env, stream));

  std::string type = Vector::StdStringOf(
      env, Symbol::name(env, Type::MapClassSymbol(Type::TypeOf(env, fn))));

  std::string ns =
      Function::IsNative(env, fn)
          ? Vector::StdStringOf(
                env,
                Namespace::name(env, Symbol::ns(env, Function::form(env, fn))))
          : "";

  std::string name =
      Function::IsNative(env, fn)
          ? Vector::StdStringOf(env, Symbol::name(env, Function::form(env, fn)))
          : std::string{":lambda"};

  std::string scope =
      (ns == "") ? ""
      : Namespace::Map(env, Symbol::ns(env, Function::form(env, fn)),
                       SCOPE::EXTERN,
                       Symbol::name(env, Function::form(env, fn)))
              .has_value()
          ? ":"
          : "::";

  std::stringstream fid;

  fid << std::dec << Fixnum::Int64Of(Function::frame_id(env, fn));

  std::stringstream hexs;

  hexs << std::hex << std::to_underlying(fn);

  Env::Write(env,
             "#<:" + type + " id:" + fid.str() + " #x" + hexs.str() + " (" +
                 ns + scope + name + ")>",
             stream, false);
}

/** * make view of function **/
Tag Function::View(Env &env, Tag fn) {
  assert(IsType(env, fn));

  std::vector<Tag> view = std::vector<Tag>{
      arity(env, fn), form(env, fn), frame_id(env, fn), extension(env, fn)};

  return Vector(view).Heap(env);
}

/** * clone function object **/
Tag Function::Heap(Env &env) {
  auto alloc = env.heap->Alloc(sizeof(Layout), SYS_CLASS::FUNCTION);
  if (!alloc.has_value())
    throw std::runtime_error("heap exhausted");

  Tag tag = Entag(alloc.value(), TAG::INDIRECT);
  *env.heap->Layout<Layout>(env, tag) = function_;

  return tag;
}

/** * clone function object **/
Tag Function::Clone(Env &env, Tag view) {
  assert(Vector::IsType(env, view));

  Function fn = Function();

  if (Vector::Length(env, view) != 4)
    Exception::Raise(env, "clone", "error", "size", view);

  fn.function_.arity = Vector::Ref<Tag>(env, view, 0);
  fn.function_.form = Vector::Ref<Tag>(env, view, 1);
  fn.function_.frame_id = Vector::Ref<Tag>(env, view, 2);
  fn.function_.extension = Vector::Ref<Tag>(env, view, 3);

  return fn.Heap(env);
}

} /* namespace type */
} /* namespace libmu */
