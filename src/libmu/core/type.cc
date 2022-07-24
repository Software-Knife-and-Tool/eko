/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  type.cc: library tagged pointers
 **
 **/
#include "libmu/core/type.h"

#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <vector>

#include "libmu/core/env.h"

#include "libmu/type/char.h"
#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/stream.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

namespace libmu {

using Char = type::Char;
using Cons = type::Cons;
using Double = type::Double;
using Exception = type::Exception;
using Fixnum = type::Fixnum;
using Float = type::Float;
using Function = type::Function;
using Namespace = type::Namespace;
using Stream = type::Stream;
using Symbol = type::Symbol;
using Vector = type::Vector;

namespace core {
namespace {

static const std::map<Tag, SYS_CLASS> kSymbolMap{
    {Symbol::Keyword("byte"), SYS_CLASS::BYTE},
    {Symbol::Keyword("char"), SYS_CLASS::CHAR},
    {Symbol::Keyword("cons"), SYS_CLASS::CONS},
    {Symbol::Keyword("double"), SYS_CLASS::DOUBLE},
    {Symbol::Keyword("except"), SYS_CLASS::EXCEPTION},
    {Symbol::Keyword("fixnum"), SYS_CLASS::FIXNUM},
    {Symbol::Keyword("float"), SYS_CLASS::FLOAT},
    {Symbol::Keyword("func"), SYS_CLASS::FUNCTION},
    {Symbol::Keyword("ns"), SYS_CLASS::NAMESPACE},
    {Symbol::Keyword("stream"), SYS_CLASS::STREAM},
    {Symbol::Keyword("symbol"), SYS_CLASS::SYMBOL},
    {Symbol::Keyword("t"), SYS_CLASS::T},
    {Symbol::Keyword("vector"), SYS_CLASS::VECTOR}};

} /* anonymous namespace */

/** * make a view vector of pointer's contents **/
Tag Type::View(Env &env, Tag object) {
  static const std::map<SYS_CLASS, std::function<Tag(Env &, Tag)>> kViewMap{
      {SYS_CLASS::EXCEPTION, Exception::View},
      {SYS_CLASS::FUNCTION, Function::View},
      {SYS_CLASS::NAMESPACE, Namespace::View},
      {SYS_CLASS::STREAM, Stream::View},
      {SYS_CLASS::VECTOR, Vector::View}};

  return kViewMap.contains(Type::TypeOf(env, object))
             ? kViewMap.at(Type::TypeOf(env, object))(env, object)
             : Type::NIL;
}

/** * is type a class symbol? **/
bool Type::IsClassSymbol(Tag type_sym) {
  assert(Symbol::IsKeyword(type_sym));

  return kSymbolMap.contains(type_sym);
}

/** * type symbol map **/
SYS_CLASS Type::MapSymbolClass(Tag type_sym) {
  assert(Symbol::IsKeyword(type_sym));
  assert(kSymbolMap.contains(type_sym));

  return kSymbolMap.at(type_sym);
}

/** * class symbol map */
Tag Type::MapClassSymbol(SYS_CLASS sys_class) {
  static const std::map<SYS_CLASS, Tag> kTypeMap{
      {SYS_CLASS::BYTE, Symbol::Keyword("byte")},
      {SYS_CLASS::CHAR, Symbol::Keyword("char")},
      {SYS_CLASS::CONS, Symbol::Keyword("cons")},
      {SYS_CLASS::DOUBLE, Symbol::Keyword("double")},
      {SYS_CLASS::EXCEPTION, Symbol::Keyword("except")},
      {SYS_CLASS::FIXNUM, Symbol::Keyword("fixnum")},
      {SYS_CLASS::FLOAT, Symbol::Keyword("float")},
      {SYS_CLASS::FUNCTION, Symbol::Keyword("func")},
      {SYS_CLASS::NAMESPACE, Symbol::Keyword("ns")},
      {SYS_CLASS::STREAM, Symbol::Keyword("stream")},
      {SYS_CLASS::SYMBOL, Symbol::Keyword("symbol")},
      {SYS_CLASS::T, Symbol::Keyword("t")},
      {SYS_CLASS::VECTOR, Symbol::Keyword("vector")}};

  assert(kTypeMap.contains(sys_class));

  return kTypeMap.at(sys_class);
}

/** * type of tagged pointer **/
SYS_CLASS Type::TypeOf(Env &env, Tag ptr) {

  switch (TagOf(ptr)) {
  case TAG::CONS:
    return SYS_CLASS::CONS;
  case TAG::DIRECT:
    return SYS_CLASS(DirectClass(ptr));
  case TAG::INDIRECT:
    return SYS_CLASS(Heap::SysClass(*env.heap->Map(ptr)));
  case TAG::UNUSED:
    [[fallthrough]];
  default:
    throw std::runtime_error("type-of botch");
  }
}

} /* namespace core */
} /* namespace libmu */
