/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  read.cc: library reader
 **
 **/
#include <cassert>
#include <limits>
#include <map>
#include <optional>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/readtable.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"

#include "libmu/type/char.h"
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

using SYNTAX_TYPE = ReadTable::SYNTAX_TYPE;
using SYNTAX_CHAR = ReadTable::SYNTAX_CHAR;

namespace {

bool is_char_syntax_type(Tag ch, SYNTAX_TYPE type) {
  return !Type::Null(ch) && ReadTable::IsCharSyntaxType(ch, type);
}

bool is_char_syntax_char(Tag ch, SYNTAX_CHAR syntax_char) {
  return !Type::Null(ch) && ReadTable::IsCharSyntaxChar(ch, syntax_char);
}

/** * read atom **/
std::string read_atom(Env &env, Tag stream) {
  assert(Stream::IsType(stream));

  std::string string{};

  Tag ch = Stream::ReadChar(env, stream);
  for (int len = 0; is_char_syntax_type(ch, SYNTAX_TYPE::CONSTITUENT); len++) {
    if (len >= Vector::MAX_LENGTH)
      Exception::Raise(env, "rd-atom", "error", "size", Type::NIL);

    string.push_back(static_cast<char>(Char::UInt64Of(ch)));
    ch = Stream::ReadChar(env, stream);
  }

  if (!Type::Null(ch))
    Stream::UnReadChar(env, ch, stream);

  if (string.size() == 0)
    Exception::Raise(env, "rd-atom", "error", "parse", Type::NIL);

  return string;
}

/** * parse a numeric std::string **/
std::optional<Tag> parse_number(Env &env, const std::string &str) {
  try {
    size_t n = 0;
    size_t fxval = std::stoull(str, &n, 0);

    if (n == str.length()) {
      if (((fxval >> 62) & 1) ^ ((fxval >> 63) & 1))
        Exception::Raise(env, "rd-num", "error", "range",
                         Vector(str).Heap(env));
      return Fixnum(fxval).tag_;
    } else {
      float flval = std::stof(str, &n);

      if (n == str.length())
        return Float(flval).tag_;
      return std::nullopt;
    }
  } catch (std::invalid_argument &ex) {
    return std::nullopt;
  } catch (std::out_of_range &ex) {
    Exception::Raise(env, "rd-num", "error", "syntax", Vector(str).Heap(env));
  } catch (std::exception &ex) {
    Exception::Raise(env, "rd-num", "error", "syntax", Vector(str).Heap(env));
  }
}

/** * read radixed fixnum from stream **/
Tag radix_fixnum(Env &env, int radix, Tag stream) {
  assert(Stream::IsType(stream));

  std::string str = read_atom(env, stream);

  try {
    size_t n;
    size_t fxval = std::stoull(str, &n, radix);

    if (n == str.length()) {
      if (((fxval >> 62) & 1) ^ ((fxval >> 63) & 1))
        Exception::Raise(env, "rd-rdx", "error", "range",
                         Vector(str).Heap(env));
      return Fixnum(fxval).tag_;
    }
    Exception::Raise(env, "rd-rdx", "error", "syntax", Vector(str).Heap(env));
  } catch (std::invalid_argument &ex) {
    Exception::Raise(env, "rd-rdx", "error", "syntax", Vector(str).Heap(env));
  } catch (std::out_of_range &ex) {
    Exception::Raise(env, "rd-rdx", "error", "range", Vector(str).Heap(env));
  } catch (std::exception &ex) {
    Exception::Raise(env, "rd-rdx", "error", "syntax", Vector(str).Heap(env));
  }
}

} /* anonymous namespace */

/** * absorb whitespace until eof **/
bool Env::ReadWSUntilEof(Env &env, Tag stream) {
  assert(Stream::IsType(stream));

  Tag ch;

  do {
    ch = Stream::ReadChar(env, stream);
    if (is_char_syntax_char(ch, SYNTAX_CHAR::SEMICOLON))
      do
        ch = Stream::ReadChar(env, stream);
      while (!Type::Null(ch) && Char::UInt64Of(ch) != '\n');
  } while (is_char_syntax_type(ch, SYNTAX_TYPE::WSPACE));

  if (Type::Null(ch))
    return false;

  Stream::UnReadChar(env, ch, stream);
  return true;
}

/** * read form **/
Tag Env::Read(Env &env, Tag stream) {
  assert(Stream::IsType(stream));

  if (!ReadWSUntilEof(env, stream))
    return Type::NIL;

  Tag ch = Stream::ReadChar(env, stream);

  if (ReadTable::IsCharSyntaxCharAny(ch)) {
    switch (ReadTable::MapSyntaxChar(ch)) {
    case SYNTAX_CHAR::SEMICOLON: /* line comment */
      do
        ch = Stream::ReadChar(env, stream);
      while (!Type::Null(ch) && Char::UInt64Of(ch) != '\n');
      return Type::Null(ch) ? Type::NIL : Read(env, stream);
    case SYNTAX_CHAR::LPAREN: /* list syntax */
      return Cons::Read(env, stream);
    case SYNTAX_CHAR::DOT: /* dot syntax */
      return ReadTable::MakeSyntax(ch);
    case SYNTAX_CHAR::QUOTE: { /* quote syntax */
      Tag quoted = Read(env, stream);
      return Symbol::IsType(quoted) || Cons::IsType(quoted)
                 ? Cons::List(
                       env, std::vector<Tag>{Symbol::Keyword("quote"), quoted})
                 : quoted;
    }
    case SYNTAX_CHAR::DQUOTE: /* string syntax */
      return Vector::ReadString(env, stream);
    case SYNTAX_CHAR::SHARP: /* sharpsign */
      ch = Stream::ReadChar(env, stream);
      if (Type::Null(ch))
        Exception::Raise(env, "read", "error", "eof", stream);

      switch (Char::UInt64Of(ch)) {
      case 'x':
        return radix_fixnum(env, 16, stream);
      case 'd':
        return radix_fixnum(env, 10, stream);
      }

      if (!ReadTable::IsCharSyntaxCharAny(ch))
        Exception::Raise(env, "read", "error", "type", ch);

      switch (ReadTable::MapSyntaxChar(ch)) {
      case SYNTAX_CHAR::BACKSLASH: /* char syntax */
        return Stream::ReadChar(env, stream);
      case SYNTAX_CHAR::LPAREN: /* vector syntax */
        return Vector::Read(env, stream);
      case SYNTAX_CHAR::COLON: { /* uninterned symbol */
        std::string name = read_atom(env, stream);
        std::optional<Tag> n = parse_number(env, name);

        if (n.has_value())
          Exception::Raise(env, "read", "error", "type",
                           Vector(name).Heap(env));

        if (name.contains(':'))
          Exception::Raise(env, "read", "error", "pkg", Vector(name).Heap(env));

        return Symbol(Type::NIL, Vector(name).Heap(env), Type::UNBOUND)
            .Heap(env);
      }
      case SYNTAX_CHAR::VBAR: /* block comment */
        for (;;) {
          ch = Stream::ReadChar(env, stream);
          if (Type::Null(ch))
            break;
          if (Char::UInt64Of(ch) == '|') {
            ch = Stream::ReadChar(env, stream);
            if (Type::Null(ch))
              break;
            if (Char::UInt64Of(ch) == '#')
              break;
            Stream::UnReadChar(env, ch, stream);
          }
        }

        if (Type::Null(ch))
          Exception::Raise(env, "read", "error", "eof", ch);

        return Read(env, stream);
      default:
        Exception::Raise(env, "read", "error", "type", ch);
      }
      break;
    case SYNTAX_CHAR::RPAREN:
      Exception::Raise(env, "read", "error", "syntax",
                       Symbol::Keyword("rparen"));
    default: {
      Stream::UnReadChar(env, ch, stream);
      std::string atom = read_atom(env, stream);
      std::optional<Tag> n = parse_number(env, atom);

      if (n.has_value())
        return n.value();

      return Symbol::ParseSymbol(env, atom);
    }
    }
  } else {
    Stream::UnReadChar(env, ch, stream);

    std::string atom = read_atom(env, stream);
    std::optional<Tag> n = parse_number(env, atom);

    if (n.has_value())
      return n.value();

    return Symbol::ParseSymbol(env, atom);
  }
}

} /* namespace core */
} /* namespace libmu */
