/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  vector.cc: library vectors
 **
 **/
#include "libmu/type/vector.h"

#include <cassert>
#include <functional>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/char.h"
#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/stream.h"

namespace libmu {

using Heap = core::Heap;

namespace type {
namespace {

template <typename T>
Tag list_to_vec(Env &env, const std::function<bool(Env &, Tag)> &isType,
                const std::function<T(Tag)> &unbox, Tag list) {
  assert(Cons::IsList(env, list));
  std::vector<T> vec;

  Cons::iter iter(env, list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    Tag car = Cons::car(env, it);

    if (!isType(env, car))
      Exception::Raise(env, "sv-list", "error", "type", car);

    vec.push_back(unbox(car));
  }

  return Vector(vec).Heap(env);
}

} /* anonymous namespace */

void *Vector::DataV(Env &env, Tag &vector) {
  assert(IsType(env, vector));

  return IsDirect(vector) ? reinterpret_cast<void *>(
                                reinterpret_cast<uint8_t *>(&vector) + 1)
                          : reinterpret_cast<void *>(
                                env.heap->HeapAddr(offset(env, vector)));
}

bool Vector::IsType(Env &env, Tag ptr) {
  return (IsDirect(ptr) && DirectClass(ptr) == DIRECT_CLASS::VECTOR) ||
         (IsIndirect(ptr) &&
          (env.heap->SysClass(env, ptr) == SYS_CLASS::VECTOR));
}

/** * vector slice **/
Tag Vector::Slice(Env &env, Tag vec, Tag off, Tag len) {
  assert(IsType(env, vec));
  assert(Fixnum::IsType(off));
  assert(Fixnum::IsType(len));

  int64_t offset = Fixnum::Int64Of(off);
  int64_t length = Fixnum::Int64Of(len);

  if (offset < 0)
    Exception::Raise(env, "slice", "range", "range", off);

  if (length < 0)
    Exception::Raise(env, "slice", "range", "range", len);

  if (static_cast<size_t>(length) + offset > Length(env, vec))
    Exception::Raise(env, "slice", "error", "range", Type::NIL);

  if (Type::IsDirect(vec) && TypeOf(env, vec) == SYS_CLASS::CHAR) {

    uint64_t bits = std::to_underlying(vec) >> (1 + offset) * 8;
    uint64_t mask = static_cast<uint64_t>(-1) << length * 8;

    return Type::MakeDirect(bits & ~mask, length, DirectClass(vec));
  }

  std::optional<size_t> alloc =
      env.heap->Alloc(sizeof(Layout), SYS_CLASS::VECTOR);

  if (!alloc.has_value())
    throw std::runtime_error("heap exhausted");

  int type_size = 0;

  switch (Vector::TypeOf(env, vec)) {
  case SYS_CLASS::BYTE:
    [[fallthrough]];
  case SYS_CLASS::CHAR:
    type_size = sizeof(char);
    break;
  case SYS_CLASS::FLOAT:
    type_size = sizeof(float);
    break;
  case SYS_CLASS::FIXNUM:
    [[fallthrough]];
  case SYS_CLASS::T:
    type_size = sizeof(Tag);
    break;
  default:
    throw std::runtime_error("vector type botch");
  }

  Layout *vlayout = Heap::Layout<Layout>(env, vec);
  Layout *nlayout =
      env.heap->Layout<Layout>(env, Entag(alloc.value(), TAG::INDIRECT));

  *nlayout = *vlayout;

  nlayout->length = len;

  auto dst = Data<Tag>(env, vec);
  nlayout->offset =
      Fixnum(reinterpret_cast<size_t>(env.heap->HeapAddr(
                 reinterpret_cast<size_t>(dst) + offset * type_size)))
          .tag_;

  return Entag(alloc.value(), TAG::INDIRECT);
}

/** * list to vector **/
Tag Vector::ListToVector(Env &env, Tag vectype, Tag list) {
  assert(Cons::IsList(env, list));

  SYS_CLASS vtype = Type::MapSymbolClass(vectype);

  switch (vtype) {
  case SYS_CLASS::T:
    return list_to_vec<Tag>(
        env, [](Env &, Tag) { return true; }, [](Tag t) { return t; }, list);
  case SYS_CLASS::FLOAT:
    return list_to_vec<float>(
        env, [](Env &, Tag t) { return Float::IsType(t); },
        [](Tag t) { return Float::FloatOf(t); }, list);
  case SYS_CLASS::FIXNUM:
    return list_to_vec<int64_t>(
        env, [](Env &, Tag t) { return Fixnum::IsType(t); },
        [](Tag t) { return Fixnum::Int64Of(t); }, list);
  case SYS_CLASS::BYTE:
    return list_to_vec<uint8_t>(
        env,
        [](Env &, Tag t) -> bool {
          return Fixnum::IsType(t) && Fixnum::Int64Of(t) < 256;
        },
        [](Tag t) { return static_cast<uint8_t>(Fixnum::Int64Of(t)); }, list);
  case SYS_CLASS::CHAR:
    return list_to_vec<char>(
        env, [](Env &, Tag t) -> bool { return Char::IsType(t); },
        [](Tag t) { return Char::UInt64Of(t); }, list);
  default:
    throw std::runtime_error("vector type violation");
  }
}

/** * print vector to stream **/
void Vector::Write(Env &env, Tag vector, Tag stream, bool esc) {
  assert(IsType(env, vector));
  assert(Stream::IsType(env, stream));

  switch (Vector::TypeOf(env, vector)) {
  case SYS_CLASS::CHAR: {
    if (esc)
      Env::Write(env, "\"", stream, false);

    Vector::iter<uint8_t> iter(env, vector);
    for (auto it = iter.begin(); it != iter.end(); it = ++iter)
      Char::Write(env, Char(*iter).tag_, stream, false);

    if (esc)
      Env::Write(env, "\"", stream, false);
    break;
  }
  case SYS_CLASS::BYTE: {
    Env::Write(env, "#(:byte", stream, false);

    Vector::iter<uint8_t> iter(env, vector);
    for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
      Env::Write(env, " ", stream, false);
      Env::Write(env, Fixnum(*iter).tag_, stream, esc);
    }

    Env::Write(env, ")", stream, false);
    break;
  }
  case SYS_CLASS::FIXNUM: {
    Env::Write(env, "#(:fixnum", stream, false);

    Vector::iter<int64_t> iter(env, vector);
    for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
      Env::Write(env, " ", stream, false);
      Env::Write(env, Fixnum(*iter).tag_, stream, esc);
    }

    Env::Write(env, ")", stream, false);
    break;
  }
  case SYS_CLASS::FLOAT: {
    Env::Write(env, "#(:float", stream, false);

    Vector::iter<float> iter(env, vector);
    for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
      Env::Write(env, " ", stream, false);
      Env::Write(env, Float(*iter).tag_, stream, esc);
    }

    Env::Write(env, ")", stream, false);
    break;
  }
  case SYS_CLASS::T: {
    Env::Write(env, "#(:t", stream, false);

    Vector::iter<Tag> iter(env, vector);
    for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
      Env::Write(env, " ", stream, false);
      Env::Write(env, *iter, stream, esc);
    }

    Env::Write(env, ")", stream, false);
    break;
  }
  default:
    throw std::runtime_error("vector type violation");
  }
}

/** * heap **/
Tag Vector::Heap(Env &env) {

  if (Type::MapSymbolClass(vector_.type) == SYS_CLASS::CHAR) {
    if (Fixnum::Int64Of(vector_.length) <= DIRECT_STR_MAX)
      return tag_;

    std::optional<Tag> str = env.heap->MapString(interned_);
    if (str.has_value())
      return str.value();
  }

  int nbytes = Fixnum::Int64Of(vector_.length) * sizeof_;
  std::optional<size_t> alloc =
      env.heap->Alloc(sizeof(Layout) + nbytes, SYS_CLASS::VECTOR);

  if (!alloc.has_value())
    throw std::runtime_error("heap exhausted");

  Layout *layout =
      env.heap->Layout<Layout>(env, Entag(alloc.value(), TAG::INDIRECT));

  uint64_t data = Fixnum::Int64Of(vector_.offset);

  void *dst = reinterpret_cast<void *>(
      reinterpret_cast<char *>(layout) +
      (Heap::HeapWords(sizeof(Layout) * sizeof(uint64_t))));

  std::memcpy(dst, reinterpret_cast<void *>(data), nbytes);

  vector_.offset = Fixnum(reinterpret_cast<size_t>(env.heap->HeapAddr(
                              reinterpret_cast<size_t>(dst))))
                       .tag_;

  *layout = vector_;

  Tag vec = Entag(alloc.value(), TAG::INDIRECT);

  if (Type::MapSymbolClass(vector_.type) == SYS_CLASS::CHAR)
    env.heap->InternString(env, vec);

  return vec;
}

/** * view of vector object **/
Tag Vector::View(Env &env, Tag vector) {
  assert(IsType(env, vector));

  std::vector<Tag> view =
      std::vector<Tag>{VecType(env, vector), Fixnum(length(env, vector)).tag_,
                       Fixnum(size_t{offset(env, vector)}).tag_};

  return Vector(view).Heap(env);
}

/** * garbage collection **/
/** * this is so wrong it makees my eyes water **/
void Vector::Gc(Env &env, Tag vec) {
  assert(IsType(env, vec));

  if (!Type::IsDirect(vec)) {
    switch (Vector::TypeOf(env, vec)) {
    case SYS_CLASS::CHAR:
    case SYS_CLASS::FIXNUM:
      [[fallthrough]];
    case SYS_CLASS::FLOAT:
      break;
    case SYS_CLASS::BYTE:
      [[fallthrough]];
    case SYS_CLASS::T: {
      if (Env::IsGcMarked(env, vec))
        return;

      Env::GcMark(env, vec);

      Vector::iter<Tag> iter(env, vec);
      for (auto it = iter.begin(); it != iter.end(); it = ++iter)
        Env::Gc(env, *it);

      break;
    }
    default:
      throw std::runtime_error("vector type botch");
    }
  }
}

/** * vector parser **/
Tag Vector::Read(Env &env, Tag stream) {
  assert(Stream::IsType(env, stream));

  Tag vectype = Env::Read(env, stream);

  if (!Symbol::IsKeyword(vectype))
    Exception::Raise(env, "vc-read", "error", "type", vectype);

  return ListToVector(env, vectype, Cons::Read(env, stream));
}

/** * read string **/
Tag Vector::ReadString(Env &env, Tag stream) {
  assert(Stream::IsType(env, stream));

  std::string str;

  Tag ch = Stream::ReadChar(env, stream);
  for (int len = 0; !core::ReadTable::IsCharSyntaxChar(
           ch, core::ReadTable::SYNTAX_CHAR::DQUOTE);
       len++) {
    if (len >= Vector::MAX_LENGTH)
      Exception::Raise(env, "vc-rdst", "error", "size", stream);

    if (core::ReadTable::IsCharSyntaxChar(
            ch, core::ReadTable::SYNTAX_CHAR::BACKSLASH)) {
      ch = Stream::ReadChar(env, stream);
      if (Type::Null(ch))
        Exception::Raise(env, "vc-rdst", "error", "eof", stream);
    }

    str.push_back(Char::UInt64Of(ch));
    ch = Stream::ReadChar(env, stream);

    if (Type::Null(ch))
      Exception::Raise(env, "vc-rdst", "error", "eof", stream);
  }

  return Vector(str).Heap(env);
}

} /* namespace type */
} /* namespace libmu */
