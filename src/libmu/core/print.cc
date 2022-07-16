/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  print.cc: library printer
 **
 **/
#include <bitset>
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <utility>

#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/char.h"
#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/namespace.h"
#include "libmu/type/stream.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

namespace libmu {

using Char = type::Char;
using Cons = type::Cons;
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

/** * print object in broket syntax to stream **/
// NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
void print_as_broket(Env &env, Tag object, Tag stream) {

  std::string type = Vector::StdStringOf(
      env, Symbol::name(env, Type::MapClassSymbol(Type::TypeOf(env, object))));
  std::stringstream hexs;

  hexs << std::hex << std::to_underlying(object);
  Env::Write(env, "#<:" + type + " #x" + hexs.str() + "()>", stream, false);
}

} /* anonymous namespace */

/** * print std::string to stream **/
void Env::Write(Env &env, const std::string &str, Tag stream, bool esc) {
  assert(Stream::IsType(env, stream));

  if (esc)
    Write(env, "\"", stream, false);

  for (auto ch : str)
    Stream::WriteByte(env, Fixnum(ch).tag_, stream);

  if (esc)
    Write(env, "\"", stream, false);
}

/** * print object to stream **/
// NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
void Env::Write(Env &env, Tag object, Tag stream, bool esc) {

  static const std::map<SYS_CLASS, std::function<void(Env &, Tag, Tag, bool)>>
      kWriteMap{{SYS_CLASS::CHAR, Char::Write},
                {SYS_CLASS::CONS, Cons::Write},
                {SYS_CLASS::FIXNUM, Fixnum::Write},
                {SYS_CLASS::FLOAT, Float::Write},
                {SYS_CLASS::FUNCTION, Function::Write},
                {SYS_CLASS::NAMESPACE, Namespace::Write},
                {SYS_CLASS::SYMBOL, Symbol::Write},
                {SYS_CLASS::VECTOR, Vector::Write}};

  if (kWriteMap.contains(Type::TypeOf(env, object)))
    kWriteMap.at(Type::TypeOf(env, object))(env, object, stream, esc);
  else
    print_as_broket(env, object, stream);
}

} /* namespace core */
} /* namespace libmu */
