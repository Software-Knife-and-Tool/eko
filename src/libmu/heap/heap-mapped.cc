/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  heap-mapped.cc: mapped heap
 **
 **/
#include "libmu/heap/mapped.h"

#include <cassert>
#include <optional>

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/fixnum.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

#include "libmu/system/internals.h"

namespace libmu {

using Fixnum = type::Fixnum;
using Heap = core::Heap;
using SYS_CLASS = core::Type::SYS_CLASS;
using Tag = core::Type::Tag;
using Vector = type::Vector;

namespace heap {

namespace {
Tag hash_id(std::string &str) {
  static const int FNV_prime = 16777619;
  static const uint64_t OFFSET_BASIS = 2166136261UL;

  int64_t hash = OFFSET_BASIS;

  for (auto it = str.begin(); it != str.end(); ++it) {
    hash ^= *it;
    hash *= FNV_prime;
  }

  return Fixnum(hash).tag_;
}

Tag hash_id(Env &env, Tag str) {
  static const int FNV_prime = 16777619;
  static const uint64_t OFFSET_BASIS = 2166136261UL;

  assert(Vector::IsTyped(env, str, Type::SYS_CLASS::CHAR));

  int64_t hash = OFFSET_BASIS;

  Vector::iter<char> iter(env, str);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter) {
    hash ^= *it;
    hash *= FNV_prime;
  }

  return Fixnum(hash).tag_;
}

} /* anonymous namespace */

/** * string interning **/
std::optional<Tag> Mapped::MapString(std::string &str) {
  Tag id = hash_id(str);

  if (interned_strings.contains(id))
    return interned_strings.at(id);

  return std::nullopt;
}

Tag Mapped::InternString(Env &env, Tag str) {
  Tag id = hash_id(env, str);

  if (interned_strings.contains(id))
    return interned_strings.at(id);

  interned_strings[id] = str;
  return str;
}

size_t Mapped::HeapInfoTag(HeapInfo *hp) {
  return heap_addr - reinterpret_cast<size_t>(hp + size_t{1});
}

/** * allocate heap object **/
std::optional<size_t> Mapped::Alloc(int nbytes, SYS_CLASS tag) {
  // NOLINTNEXTLINE(performance-no-int-to-ptr)
  HeapInfo *halloc = reinterpret_cast<HeapInfo *>(alloc_barrier);
  size_t nalloc = sizeof(HeapInfo) + HeapWords(nbytes) * sizeof(uint64_t);

  if (alloc_barrier + nalloc > heap_addr + HeapSize())
    return std::nullopt;

  *halloc = MakeHeapInfo(nalloc, tag);

  alloc_barrier += nalloc;

  /* metrics */
  n_objects++;
  type_alloc->at(std::to_underlying(tag))++;

  return heap_addr - reinterpret_cast<size_t>(halloc + 1);
}

/** * count up total data bytes in heap **/
size_t Mapped::Room() {
  size_t nbytes = 0;

  heapinfo_iter iter(this);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    nbytes += Size(*it);

  return nbytes;
}

/** * count up total type tag size **/
size_t Mapped::Room(SYS_CLASS tag) {
  size_t total_size = 0;

  heapinfo_iter iter(this);
  for (auto it = iter.begin(); it != iter.end(); it = ++iter)
    if (tag == SysClass(*it))
      total_size += Size(*it);

  return total_size;
}

/** * heap object **/
Mapped::Mapped(system::System &sys, size_t n_pages)
    : sys(sys), n_pages(n_pages) {
  const char *heapId = "heap";

  page_size = system::System::PAGESIZE;

  std::optional<const char *> addr = system::System::MapPages(n_pages, heapId);

  if (!addr.has_value())
    throw std::runtime_error("mmap failed");

  mapped_file_name = "";

  heap_addr = reinterpret_cast<size_t>(addr.value());
  alloc_barrier = reinterpret_cast<size_t>(addr.value());
  type_free = std::make_unique<std::vector<int>>(16, 0);
  type_alloc = std::make_unique<std::vector<int>>(16, 0);
}

} /* namespace heap */
} /* namespace libmu */
