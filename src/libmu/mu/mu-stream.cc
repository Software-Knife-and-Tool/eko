/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 ** mu-stream.cc: library stream functions
 **
 **/
#include <sys/stat.h>

#include <cassert>
#include <fstream>
#include <iostream>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/readtable.h"
#include "libmu/core/type.h"

#include "libmu/type/char.h"
#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/namespace.h"
#include "libmu/type/stream.h"

namespace libmu {

using Char = type::Char;
using Cons = type::Cons;
using Exception = type::Exception;
using Fixnum = type::Fixnum;
using Float = type::Float;
using Function = type::Function;
using Stream = type::Stream;
using Symbol = type::Symbol;
using Vector = type::Vector;

using Context = core::Context;
using Env = core::Env;
using Frame = core::Context::Frame;
using Tag = core::Tag;
using Type = core::Type;

namespace mu {

/** * (eofp stream) **/
void IsEof(Context &ctx, Frame &fp) {
  Tag stream = fp.argv[0];

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "eofp", "error", "type", fp.argv[0]);

  fp.value = Type::Bool(Stream::IsEof(ctx.env, stream));
}

/** * (un-byte fixnum stream) => fixnum **/
void UnReadByte(Context &ctx, Frame &fp) {
  Tag ch = fp.argv[0];
  Tag stream = fp.argv[1];

  if (!Fixnum::IsType(ch))
    Exception::Raise(ctx.env, "un-byte", "error", "type", ch);

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "un-byte", "error", "type", fp.argv[1]);

  fp.value = Stream::UnReadByte(ctx.env, ch, stream);
}

/** * (un-char ch stream) => char **/
void UnReadChar(Context &ctx, Frame &fp) {
  Tag ch = fp.argv[0];
  Tag stream = fp.argv[1];

  if (!Char::IsType(ch))
    Exception::Raise(ctx.env, "un-char", "error", "type", ch);

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "un-char", "error", "type", fp.argv[1]);

  fp.value = Stream::UnReadChar(ctx.env, ch, stream);
}

/** * (rd-byte stream) => fixnum **/
void ReadByte(Context &ctx, Frame &fp) {
  Tag stream = fp.argv[0];

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "rd-byte", "error", "ftype", fp.argv[0]);

  fp.value = Stream::ReadByte(ctx.env, stream);
}

/** * (rd-char stream) => fixnum **/
void ReadChar(Context &ctx, Frame &fp) {
  Tag stream = fp.argv[0];

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "rd-byte", "error", "ftype", fp.argv[0]);

  fp.value = Stream::ReadChar(ctx.env, stream);
}

/** * (wr-byte fixnum stream) => fixnum  **/
void WriteByte(Context &ctx, Frame &fp) {
  Tag byte = fp.argv[0];
  Tag stream = fp.argv[1];

  if (!Fixnum::IsType(byte))
    Exception::Raise(ctx.env, "wr-byte", "error", "type", byte);

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "wr-byte", "error", "type", stream);

  Stream::WriteByte(ctx.env, byte, stream);

  fp.value = byte;
}

/** * (wr-char fixnum stream) => char **/
void WriteChar(Context &ctx, Frame &fp) {
  Tag ch = fp.argv[0];
  Tag stream = fp.argv[1];

  if (!Char::IsType(ch))
    Exception::Raise(ctx.env, "wr-char", "error", "type", ch);

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "wr-char", "error", "type", stream);

  Stream::WriteChar(ctx.env, ch, stream);

  fp.value = ch;
}

/** * (open :type :direction string) => stream **/
void OpenStream(Context &ctx, Frame &fp) {
  static const int OPEN_FILE = 1;
  static const int OPEN_STRING = 0;
  static const int OPEN_INPUT = 2;
  static const int OPEN_OUTPUT = 0;

  Tag type = fp.argv[0];
  Tag dir = fp.argv[1];
  Tag str = fp.argv[2];

  if (!Type::Eq(type, Symbol::Keyword("file")) &&
      !Type::Eq(type, Symbol::Keyword("string")))
    Exception::Raise(ctx.env, "open", "error", "type", type);

  if (!Type::Eq(dir, Symbol::Keyword("input")) &&
      !Type::Eq(dir, Symbol::Keyword("output")))
    Exception::Raise(ctx.env, "open", "error", "type", dir);

  if (!Vector::IsTyped(ctx.env, str, Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, "open", "error", "type", str);

  int st_type =
      Type::Eq(type, Symbol::Keyword("file")) ? OPEN_FILE : OPEN_STRING;
  int st_dir =
      Type::Eq(dir, Symbol::Keyword("input")) ? OPEN_INPUT : OPEN_OUTPUT;

  switch (st_type | st_dir) {
  case OPEN_FILE | OPEN_INPUT:
    fp.value =
        Stream::OpenInputFile(ctx.env, Vector::StdStringOf(ctx.env, str));
    break;
  case OPEN_FILE | OPEN_OUTPUT:
    fp.value =
        Stream::OpenOutputFile(ctx.env, Vector::StdStringOf(ctx.env, str));
    break;
  case OPEN_STRING | OPEN_INPUT:
    fp.value =
        Stream::OpenInputString(ctx.env, Vector::StdStringOf(ctx.env, str));
    break;
  case OPEN_STRING | OPEN_OUTPUT:
    fp.value =
        Stream::OpenOutputString(ctx.env, Vector::StdStringOf(ctx.env, str));
    break;
  default:
    assert(false);
  }
}

/** * (get-output-string-stream stream) => string **/
void GetStringStream(Context &ctx, Frame &fp) {
  Tag stream = fp.argv[0];

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "get-str", "error", "type", stream);

  if (!Stream::stream(ctx.env, stream)->IsString())
    Exception::Raise(ctx.env, "get-str", "error", "type", stream);

  fp.value =
      Vector(Stream::stream(ctx.env, stream)->GetStdString()).Heap(ctx.env);
}

/** * (close stream) =>  **/
void Close(Context &ctx, Frame &fp) {
  Tag stream = fp.argv[0];

  if (!Stream::IsType(ctx.env, stream))
    Exception::Raise(ctx.env, "close", "error", "type", stream);

  Stream::Close(ctx.env, stream);
  fp.value = Type::T;
}

} /* namespace mu */
} /* namespace libmu */
