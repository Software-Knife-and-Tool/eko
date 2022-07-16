/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  readtable.cc: library readtable
 **
 **/
#include "libmu/core/readtable.h"

#include <cassert>
#include <map>
#include <vector>

#include "libmu/type/char.h"
#include "libmu/type/fixnum.h"

namespace libmu {

using Char = type::Char;
using Fixnum = type::Fixnum;

namespace core {

/** * change this char to a syntax char **/
/** * a syntax immediate with a zero length **/
Tag ReadTable::MakeSyntax(Tag ch) {
  assert(Char::IsType(ch));

  return Type::MakeDirect(Type::DirectData(ch), 0, DIRECT_CLASS::CHAR);
}

/** * is this char any syntax type? **/
bool ReadTable::IsCharSyntaxCharAny(Tag ch) {
  static const std::string syntax_chars = "\"#`(),.:;'\\|'}";

  return Char::IsType(ch) &&
         syntax_chars.find(Type::DirectData(ch)) != std::string::npos;
}

/** * is this char a particular syntax type? **/
bool ReadTable::IsCharSyntaxChar(Tag ch, ReadTable::SYNTAX_CHAR type) {

  return IsCharSyntaxCharAny(ch) && MapSyntaxChar(ch) == type;
}

/** * is this char a particular type? **/
bool ReadTable::IsCharSyntaxType(Tag ch, ReadTable::SYNTAX_TYPE type) {

  return Char::IsType(ch) && MapSyntaxType(ch) == type;
}

/** * is this char any syntax type? **/
bool ReadTable::IsSyntaxCharAny(Tag ch) {
  static const std::string syntax_chars = "\"#`(),.:;'\\|'}";

  return IsSyntax(ch) &&
         syntax_chars.find(Type::DirectData(ch)) != std::string::npos;
}

/** * is this syntax a particular syntax type? **/
bool ReadTable::IsSyntaxChar(Tag ch, ReadTable::SYNTAX_CHAR type) {

  return IsSyntax(ch) && IsSyntaxCharAny(ch) && MapSyntaxChar(ch) == type;
}

/** * is this syntax a particular type? **/
bool ReadTable::IsSyntaxType(Tag ch, ReadTable::SYNTAX_TYPE type) {

  return IsSyntax(ch) && MapSyntaxType(ch) == type;
}

/** * map char/syntax to syntax type **/
ReadTable::SYNTAX_TYPE ReadTable::MapSyntaxType(Tag ch) {
  assert(Char::IsType(ch) || IsSyntax(ch));

  static const std::map<uint64_t, SYNTAX_TYPE> kTypeMap{
      {'\b', SYNTAX_TYPE::CONSTITUENT}, {'0', SYNTAX_TYPE::CONSTITUENT},
      {'1', SYNTAX_TYPE::CONSTITUENT},  {'2', SYNTAX_TYPE::CONSTITUENT},
      {'3', SYNTAX_TYPE::CONSTITUENT},  {'4', SYNTAX_TYPE::CONSTITUENT},
      {'5', SYNTAX_TYPE::CONSTITUENT},  {'6', SYNTAX_TYPE::CONSTITUENT},
      {'7', SYNTAX_TYPE::CONSTITUENT},  {'8', SYNTAX_TYPE::CONSTITUENT},
      {'9', SYNTAX_TYPE::CONSTITUENT},  {':', SYNTAX_TYPE::CONSTITUENT},
      {'<', SYNTAX_TYPE::CONSTITUENT},  {'>', SYNTAX_TYPE::CONSTITUENT},
      {'=', SYNTAX_TYPE::CONSTITUENT},  {'?', SYNTAX_TYPE::CONSTITUENT},
      {'!', SYNTAX_TYPE::CONSTITUENT},  {'@', SYNTAX_TYPE::CONSTITUENT},
      {'\t', SYNTAX_TYPE::WSPACE},      {'\n', SYNTAX_TYPE::WSPACE},
      {'\r', SYNTAX_TYPE::WSPACE},      {'\f', SYNTAX_TYPE::WSPACE},
      {' ', SYNTAX_TYPE::WSPACE},       {';', SYNTAX_TYPE::TMACRO},
      {'"', SYNTAX_TYPE::TMACRO},       {'#', SYNTAX_TYPE::MACRO},
      {'\'', SYNTAX_TYPE::TMACRO},      {'(', SYNTAX_TYPE::TMACRO},
      {')', SYNTAX_TYPE::TMACRO},       {'`', SYNTAX_TYPE::TMACRO},
      {',', SYNTAX_TYPE::TMACRO},       {'\\', SYNTAX_TYPE::ESCAPE},
      {'|', SYNTAX_TYPE::MESCAPE},      {'A', SYNTAX_TYPE::CONSTITUENT},
      {'B', SYNTAX_TYPE::CONSTITUENT},  {'C', SYNTAX_TYPE::CONSTITUENT},
      {'D', SYNTAX_TYPE::CONSTITUENT},  {'E', SYNTAX_TYPE::CONSTITUENT},
      {'F', SYNTAX_TYPE::CONSTITUENT},  {'G', SYNTAX_TYPE::CONSTITUENT},
      {'H', SYNTAX_TYPE::CONSTITUENT},  {'I', SYNTAX_TYPE::CONSTITUENT},
      {'J', SYNTAX_TYPE::CONSTITUENT},  {'K', SYNTAX_TYPE::CONSTITUENT},
      {'L', SYNTAX_TYPE::CONSTITUENT},  {'M', SYNTAX_TYPE::CONSTITUENT},
      {'N', SYNTAX_TYPE::CONSTITUENT},  {'O', SYNTAX_TYPE::CONSTITUENT},
      {'P', SYNTAX_TYPE::CONSTITUENT},  {'Q', SYNTAX_TYPE::CONSTITUENT},
      {'R', SYNTAX_TYPE::CONSTITUENT},  {'S', SYNTAX_TYPE::CONSTITUENT},
      {'T', SYNTAX_TYPE::CONSTITUENT},  {'V', SYNTAX_TYPE::CONSTITUENT},
      {'W', SYNTAX_TYPE::CONSTITUENT},  {'X', SYNTAX_TYPE::CONSTITUENT},
      {'Y', SYNTAX_TYPE::CONSTITUENT},  {'Z', SYNTAX_TYPE::CONSTITUENT},
      {'[', SYNTAX_TYPE::CONSTITUENT},  {']', SYNTAX_TYPE::CONSTITUENT},
      {'$', SYNTAX_TYPE::CONSTITUENT},  {'*', SYNTAX_TYPE::CONSTITUENT},
      {'{', SYNTAX_TYPE::CONSTITUENT},  {'}', SYNTAX_TYPE::CONSTITUENT},
      {'+', SYNTAX_TYPE::CONSTITUENT},  {'-', SYNTAX_TYPE::CONSTITUENT},
      {'/', SYNTAX_TYPE::CONSTITUENT},  {'~', SYNTAX_TYPE::CONSTITUENT},
      {'.', SYNTAX_TYPE::CONSTITUENT},  {'%', SYNTAX_TYPE::CONSTITUENT},
      {'&', SYNTAX_TYPE::CONSTITUENT},  {'^', SYNTAX_TYPE::CONSTITUENT},
      {'_', SYNTAX_TYPE::CONSTITUENT},  {'a', SYNTAX_TYPE::CONSTITUENT},
      {'b', SYNTAX_TYPE::CONSTITUENT},  {'c', SYNTAX_TYPE::CONSTITUENT},
      {'d', SYNTAX_TYPE::CONSTITUENT},  {'e', SYNTAX_TYPE::CONSTITUENT},
      {'f', SYNTAX_TYPE::CONSTITUENT},  {'g', SYNTAX_TYPE::CONSTITUENT},
      {'h', SYNTAX_TYPE::CONSTITUENT},  {'i', SYNTAX_TYPE::CONSTITUENT},
      {'j', SYNTAX_TYPE::CONSTITUENT},  {'k', SYNTAX_TYPE::CONSTITUENT},
      {'l', SYNTAX_TYPE::CONSTITUENT},  {'m', SYNTAX_TYPE::CONSTITUENT},
      {'n', SYNTAX_TYPE::CONSTITUENT},  {'o', SYNTAX_TYPE::CONSTITUENT},
      {'p', SYNTAX_TYPE::CONSTITUENT},  {'q', SYNTAX_TYPE::CONSTITUENT},
      {'r', SYNTAX_TYPE::CONSTITUENT},  {'s', SYNTAX_TYPE::CONSTITUENT},
      {'t', SYNTAX_TYPE::CONSTITUENT},  {'u', SYNTAX_TYPE::CONSTITUENT},
      {'v', SYNTAX_TYPE::CONSTITUENT},  {'w', SYNTAX_TYPE::CONSTITUENT},
      {'x', SYNTAX_TYPE::CONSTITUENT},  {'y', SYNTAX_TYPE::CONSTITUENT},
      {'z', SYNTAX_TYPE::CONSTITUENT}};

  uint64_t cch = IsSyntax(ch) ? UInt64Of(ch) : Char::UInt64Of(ch);

  assert(kTypeMap.contains(cch));
  return kTypeMap.at(cch);
}

/** * map a char/syntax to syntax char **/
ReadTable::SYNTAX_CHAR ReadTable::MapSyntaxChar(Tag ch) {
  assert(Char::IsType(ch) || IsSyntax(ch));

  static const std::map<uint64_t, SYNTAX_CHAR> kCharMap{
      {'"', SYNTAX_CHAR::DQUOTE},     {'#', SYNTAX_CHAR::SHARP},
      {'`', SYNTAX_CHAR::BACKQUOTE},  {'(', SYNTAX_CHAR::LPAREN},
      {')', SYNTAX_CHAR::RPAREN},     {',', SYNTAX_CHAR::COMMA},
      {'.', SYNTAX_CHAR::DOT},        {':', SYNTAX_CHAR::COLON},
      {';', SYNTAX_CHAR::SEMICOLON},  {'\'', SYNTAX_CHAR::QUOTE},
      {'\\', SYNTAX_CHAR::BACKSLASH}, {'|', SYNTAX_CHAR::VBAR}};

  uint64_t cch = IsSyntax(ch) ? UInt64Of(ch) : Char::UInt64Of(ch);
  return kCharMap.at(cch);
}

} /* namespace core */
} /* namespace libmu */
