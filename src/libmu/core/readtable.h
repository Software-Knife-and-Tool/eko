/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  readtable.h: library readtable
 **
 **/

#if !defined(LIBMU_CORE_READTABLE_H_)
#define LIBMU_CORE_READTABLE_H_

#include <cassert>
#include <map>

#include "libmu/core/type.h"

namespace libmu {
namespace core {

using Tag = Type::Tag;
using DIRECT_CLASS = Type::DIRECT_CLASS;

class ReadTable {
public:
  /** * readtable types **/
  enum class SYNTAX_TYPE {
    CONSTITUENT,
    ILLEGAL,
    WSPACE,
    MACRO,
    TMACRO,
    ESCAPE,
    MESCAPE
  };

  /** * syntax characters **/
  enum class SYNTAX_CHAR : uint64_t {
    AT = '@',
    BACKQUOTE = '`',
    BACKSLASH = '\\',
    COLON = ':',
    COMMA = ',',
    SEMICOLON = ';',
    DOT = '.',
    LPAREN = '(',
    QUOTE = '\'',
    RPAREN = ')',
    SHARP = '#',
    DQUOTE = '"',
    VBAR = '|'
  };

  /** * we got four kinds of character things:

        regular chars, numeric, symbol name constituents, etc
        syntax chars, which are regular chars in reader syntax context.
                      that we use to distinguish between #\) and ) for
                      list parsing, etc. The only syntax that produces a
                      regular char is #\.

        SYNTAX_TYPE, (MapSyntaxType) the syntax category a regular char maps to
        SYNTAX_CHAR, (MapSyntaxChar) the syntax identifier a syntax char maps to

        read gets regular chars, and dispatches on their syntax type.

        If it's a constituent, try to make a symbol or number from the
     stream.

        Otherwise, do syntax processing. Some chars, like ) will need to be
        converted to syntax chars and be returned.
  */

  /** * is this a syntax tag? **/
  static inline bool IsSyntax(Tag ch) {
    return Type::IsDirect(ch) &&
           (Type::DirectClass(ch) == DIRECT_CLASS::CHAR) &&
           (Type::DirectSize(ch) == 0);
  }

  /** * extract char from syntax **/
  static inline uint64_t UInt64Of(Tag ptr) {
    assert(IsSyntax(ptr));

    return Type::DirectData(ptr);
  }

  /** * convert a char to a syntax tag **/
  static Tag MakeSyntax(Tag);

  static bool IsCharSyntaxCharAny(Tag);
  static bool IsCharSyntaxChar(Tag, SYNTAX_CHAR);
  static bool IsCharSyntaxType(Tag, SYNTAX_TYPE);

  static bool IsSyntaxCharAny(Tag);
  static bool IsSyntaxChar(Tag, SYNTAX_CHAR);
  static bool IsSyntaxType(Tag, SYNTAX_TYPE);

  static SYNTAX_CHAR MapSyntaxChar(Tag);
  static SYNTAX_TYPE MapSyntaxType(Tag);

}; /* class ReadTable */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_CORE_READTABLE_H_ */
