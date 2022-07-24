/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 ** stream.cc: library streams
 **
 **/
#include "libmu/type/stream.h"

#include <cassert>

#include <fstream>
#include <iostream>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/readtable.h"
#include "libmu/core/type.h"

#include "libmu/type/char.h"
#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/function.h"
#include "libmu/type/namespace.h"

#include "libmu/system/internals.h"

namespace libmu {

using Heap = core::Heap;

namespace type {

/** * type predicate **/
bool Stream::IsType(Env &env, Tag ptr) {
  return Type::IsIndirect(ptr) &&
         env.heap->SysClass(env, ptr) == Type::SYS_CLASS::STREAM;
}

/** * view of stream object **/
Tag Stream::View(Env &env, Tag sp) {
  assert(IsType(env, sp));

  std::vector<Tag> view =
      std::vector<Tag>{Fixnum(reinterpret_cast<size_t>(stream(env, sp))).tag_};

  return Vector(view).Heap(env);
}

/** * stream eof predicate **/
bool Stream::IsEof(Env &env, Tag sp) {
  assert(Stream::IsType(env, sp));
  assert(stream(env, sp)->IsInput());

  return stream(env, sp)->IsEof();
}

/** * stream closed predicate **/
bool Stream::IsClosed(Env &env, Tag sp) {
  assert(Stream::IsType(env, sp));

  return stream(env, sp)->IsClosed();
}

/** * flush stream **/
void Stream::Flush(Env &env, Tag sp) {
  assert(IsType(env, sp));
  assert(stream(env, sp)->IsOutput());

  stream(env, sp)->Flush();
}

/** * unread char from stream **/
Tag Stream::UnReadByte(Env &env, Tag byte, Tag sp) {
  assert(Fixnum::IsType(byte));
  assert(Stream::IsType(env, sp));
  assert(stream(env, sp)->IsInput());

  stream(env, sp)->UnReadByte(Fixnum::Int64Of(byte));

  return byte;
}

/** * write byte to stream **/
void Stream::WriteByte(Env &env, Tag byte, Tag sp) {
  assert(Fixnum::IsType(byte));
  assert(Stream::IsType(env, sp));
  assert(stream(env, sp)->IsOutput());

  stream(env, sp)->WriteByte(static_cast<int>(Fixnum::Int64Of(byte)));
}

/** * read byte from stream, returns a fixnum or nil **/
Tag Stream::ReadByte(Env &env, Tag sp) {
  assert(IsType(env, sp));
  assert(stream(env, sp)->IsOutput());

  std::optional<int> byte = stream(env, sp)->ReadByte();

  return byte.has_value() ? Fixnum(byte.value()).tag_ : NIL;
}

/** * unread char from stream **/
Tag Stream::UnReadChar(Env &env, Tag ch, Tag sp) {
  assert(Char::IsType(ch));
  assert(Stream::IsType(env, sp));
  assert(stream(env, sp)->IsInput());

  stream(env, sp)->UnReadByte(static_cast<int64_t>(Char::UInt64Of(ch)));

  return ch;
}

/** * write char to stream **/
void Stream::WriteChar(Env &env, Tag ch, Tag sp) {
  assert(Char::IsType(ch));
  assert(Stream::IsType(env, sp));
  assert(stream(env, sp)->IsOutput());

  stream(env, sp)->WriteByte(static_cast<int>(Char::UInt64Of(ch)));
}

/** * read char from stream, returns a char or nil **/
Tag Stream::ReadChar(Env &env, Tag sp) {
  assert(IsType(env, sp));
  assert(stream(env, sp)->IsInput());

  std::optional<int> byte = stream(env, sp)->ReadByte();

  return byte.has_value() ? Char(byte.value()).tag_ : NIL;
}

/** * close stream **/
Tag Stream::Close(Env &env, Tag sp) {
  assert(IsType(env, sp));

  stream(env, sp)->Close();
  return T;
}

Tag Stream::Heap(Env &env) {
  auto alloc = env.heap->Alloc(sizeof(Layout), SYS_CLASS::STREAM);
  if (!alloc.has_value())
    throw std::runtime_error("heap exhausted");

  *reinterpret_cast<Layout *>(env.heap->HeapAddr(alloc.value())) = stream_;

  return Entag(alloc.value(), TAG::INDIRECT);
}

Stream::Stream(system::Stream *stream) : Type() { stream_.stream = stream; }

} /* namespace type */
} /* namespace libmu */
