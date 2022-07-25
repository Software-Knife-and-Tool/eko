/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  cons.cc: library conses
 **
 **/
#include "libmu/type/cons.h"

#include <cassert>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/readtable.h"
#include "libmu/core/type.h"

#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/function.h"
#include "libmu/type/stream.h"

namespace libmu {

using Env = core::Env;

using Heap = core::Heap;
using ReadTable = core::ReadTable;
using Tag = core::Type::Tag;
using Type = core::Type;

namespace type {
namespace {

std::optional<uint32_t> pack(Tag tag) {
  uint64_t tag_bits = std::to_underlying(tag);

  uint64_t sext = (tag_bits >> 31) & 1;
  uint32_t top = tag_bits >> 32;

  /* make sure we can extend tag bit 1 to tag bit 0 */
  if (((tag_bits >> 1) & 1) != (tag_bits & 1))
    return std::nullopt;

  /* make sure we can restore the msb */
  if (((tag_bits >> 30) & 1) != ((tag_bits >> 31) & 1))
    return std::nullopt;

  /* and the sign bit to the upper word */
  if ((sext && (~top != 0)) || (!sext && (top != 0)))
    return std::nullopt;

  return static_cast<uint32_t>((tag_bits >> 1) & 0x7fffffff);
}

Tag unpack_car(Tag tag) {

  uint32_t tag_bits = static_cast<int64_t>(std::to_underlying(tag)) >> 33;

  uint64_t sext = (tag_bits >> 30) & 1;
  int t0 = tag_bits & 1;

  /* restore the tag bits and upper word */
  return Tag((static_cast<uint64_t>(sext ? 0xffffffff : 0x0) << 32) |
             (sext << 31) | (tag_bits << 1) | t0);
}

Tag unpack_cdr(Tag tag) {
  uint64_t tag_bits = (std::to_underlying(tag) >> 2);

  int sext = (tag_bits >> 29) & 1;

  return unpack_car(Tag((tag_bits << 33) | static_cast<uint64_t>(sext) << 63));
}

} /* anonymous namespace */

bool Cons::IsType(Tag ptr) {
  return (TagOf(ptr) == TAG::CONS) ||
         (IsIndirect(ptr) && IndirectClass(ptr) == SYS_CLASS::CONS);
}

bool Cons::IsList(Tag ptr) { return Null(ptr) || IsType(ptr); }

Tag Cons::Heap(Env &env) {

  std::optional<uint64_t> packed = ([](Layout cons) -> std::optional<uint64_t> {
    /* these are 31 bit values */
    std::optional<uint32_t> car = pack(cons.car);
    std::optional<uint32_t> cdr = pack(cons.cdr);

    if (car.has_value() && cdr.has_value())
      return static_cast<uint64_t>((static_cast<uint64_t>(car.value()) << 33) |
                                   (cdr.value() << 2));

    return std::nullopt;
  })(cons_);

  if (packed.has_value()) {
    assert(Type::Eq(cons_.car, car(env, Entag(packed.value(), TAG::CONS))));
    assert(Type::Eq(cons_.cdr, cdr(env, Entag(packed.value(), TAG::CONS))));

    return Entag(packed.value(), TAG::CONS);
  }

  std::optional<size_t> alloc =
      Heap::GcAlloc(env, sizeof(Layout), SYS_CLASS::CONS);

  if (!alloc.has_value())
    throw std::runtime_error("heap exhausted");

  Tag tag = Entag(alloc.value(), SYS_CLASS::CONS, TAG::INDIRECT);
  *env.heap->Layout<Layout>(env, tag) = cons_;

  return tag;
}

/** * car/cdr **/
Tag Cons::car(Env &env, Tag cp) {
  assert(IsList(cp));

  return Null(cp) ? NIL
                  : (Type::IsIndirect(cp) ? Heap::Layout<Layout>(env, cp)->car
                                          : unpack_car(cp));
}

Tag Cons::cdr(Env &env, Tag cp) {
  assert(IsList(cp));

  return Null(cp) ? NIL
                  : (Type::IsIndirect(cp) ? Heap::Layout<Layout>(env, cp)->cdr
                                          : unpack_cdr(cp));
}

/** * garbage collection **/
void Cons::Gc(Env &env, Tag ptr) {
  assert(IsType(ptr));

  if (Type::IsIndirect(ptr)) {
    if (Env::IsGcMarked(env, ptr))
      return;

    Env::GcMark(env, ptr);
  }

  Env::Gc(env, car(env, ptr));
  Env::Gc(env, cdr(env, ptr));
}

/** * make a list from a std::vector **/
Tag Cons::List(Env &env, const std::vector<Tag> &src) {

  if (src.size() == 0)
    return NIL;

  Tag rlist = NIL;

  for (int nth = src.size(); nth; --nth)
    rlist = Cons(src[nth - 1], rlist).Heap(env);

  return rlist;
}

/** * make a dotted list from a std::vector<Tag> **/
Tag Cons::ListDot(Env &env, const std::vector<Tag> &src) {

  if (src.size() == 0)
    return NIL;

  Tag rlist = Cons(src[src.size() - 2], src[src.size() - 1]).Heap(env);

  if (src.size() > 2)
    for (int nth = src.size() - 2; nth != 0; --nth)
      rlist = Cons(src[nth - 1], rlist).Heap(env);

  return rlist;
}

/** * nth element of list **/
Tag Cons::Nth(Env &env, Tag list, size_t index) {
  assert(IsList(list));

  Cons::iter iter(env, list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    if (index-- == 0)
      return Cons::car(env, it);

  return NIL;
}

/** * list length **/
size_t Cons::Length(Env &env, Tag list) {
  assert(IsList(list));

  if (Null(list))
    return 0;

  int len = 0;

  Cons::iter iter(env, list);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    len++;
    if (!IsList(Cons::cdr(env, it)))
      Exception::Raise(env, "length", "error", "type", list);
  }

  return len;
}

/** * print list to stream **/
void Cons::Write(Env &env, Tag cons, Tag stream, bool esc) {
  assert(IsList(cons));
  assert(Stream::IsType(stream));

  Env::Write(env, "(", stream, false);

  Cons::iter iter(env, cons);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    if (it != iter.begin())
      Env::Write(env, " ", stream, false);

    Env::Write(env, Cons::car(env, it), stream, esc);

    Tag cdr = Cons::cdr(env, it);
    if (!Cons::IsType(cdr) && !Null(cdr)) { /* !IsList? */
      Env::Write(env, " . ", stream, false);
      Env::Write(env, cdr, stream, esc);
    }
  }

  Env::Write(env, ")", stream, false);
}

/** * list parser **/
Tag Cons::Read(Env &env, Tag stream) {
  assert(Stream::IsType(stream));

  std::vector<Tag> vlist;

  if (!Env::ReadWSUntilEof(env, stream))
    Exception::Raise(env, "co-read", "error", "eof", stream);

  if (!Env::ReadWSUntilEof(env, stream))
    Exception::Raise(env, "co-read", "error", "eof", Type::NIL);

  Tag ch = Stream::ReadChar(env, stream);
  if (ReadTable::IsCharSyntaxChar(ch, ReadTable::SYNTAX_CHAR::RPAREN))
    return Type::NIL;

  Stream::UnReadChar(env, ch, stream);

  for (;;) {
    Tag el = Env::Read(env, stream);

    if (ReadTable::IsSyntaxChar(el, ReadTable::SYNTAX_CHAR::DOT)) {
      vlist.push_back(Env::Read(env, stream));

      if (!Env::ReadWSUntilEof(env, stream))
        Exception::Raise(env, "co-read", "error", "eof", Type::NIL);

      ch = Stream::ReadChar(env, stream);
      if (!ReadTable::IsCharSyntaxChar(ch, ReadTable::SYNTAX_CHAR::RPAREN))
        Exception::Raise(env, "co-read", "error", "syntax", ch);

      return Cons::ListDot(env, vlist);
    }

    vlist.push_back(el);

    if (!Env::ReadWSUntilEof(env, stream))
      Exception::Raise(env, "co-read", "error", "eof", Type::NIL);

    ch = Stream::ReadChar(env, stream);
    if (ReadTable::IsCharSyntaxChar(ch, ReadTable::SYNTAX_CHAR::RPAREN))
      return Cons::List(env, vlist);

    Stream::UnReadChar(env, ch, stream);
  }
}

} /* namespace type */
} /* namespace libmu */
