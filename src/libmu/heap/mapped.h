/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mapped.h: mapped image heap
 **
 **/
#if !defined(LIBMU_HEAP_MAPPED_H_)
#define LIBMU_HEAP_MAPPED_H_

#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <iostream>
#include <list>
#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/system/internals.h"

namespace libmu {

using Env = core::Env;
using Tag = core::Type::Tag;
using Type = core::Type;
using SYS_CLASS = core::Type::SYS_CLASS;
using Heap = core::Heap;

namespace heap {

class Mapped : public Heap {
private: /* heap */
  template <typename T> T *Map(size_t offset) const {
    return reinterpret_cast<T *>(heap_addr + offset);
  }

public:
  system::System &sys; /* system interface */

  std::string mapped_file_name; /* mapped file */
  size_t alloc_barrier;         /* alloc barrier */
  size_t heap_addr;             /* user virtual address */
  size_t n_pages;               /* number of pages in the heap */
  size_t page_size;             /* page size for this heap */

  size_t n_objects; /* number of objects in the heap */
  std::unique_ptr<std::vector<int>> type_alloc; /* allocated type counts */
  std::unique_ptr<std::vector<int>> type_free;  /* free type counts */

public:
  size_t HeapSize() override { return page_size * n_pages; }
  size_t HeapAlloc() override { return alloc_barrier - heap_addr; }

  size_t TypeAlloc(Type::SYS_CLASS sys_class) override {
    return type_alloc->at(std::to_underlying(sys_class));
  }

  size_t TypeAlloc() override {
    return std::accumulate(type_alloc->begin(), type_alloc->end(), size_t{0},
                           std::plus<>());
  }

  std::unordered_map<Tag, Tag> interned_strings{};

  std::optional<Tag> MapString(std::string &) override;
  Tag InternString(Env &, Tag str) override;

  size_t Room() override;
  size_t Room(Type::SYS_CLASS) override;

  /** * maps **/
  HeapInfo *Map(Tag ptr) override {
    return reinterpret_cast<HeapInfo *>(HeapAddr(Type::IndirectData(ptr) - 1));
  }

  void Map(std::function<void(HeapInfo *)> fn) override {

    heapinfo_iter iter(this);
    for (auto it = iter.begin(); it != iter.end(); it = ++iter)
      fn(it);
  }

  void *HeapAddr(size_t offset) override {

    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    return reinterpret_cast<void *>(heap_addr - offset);
  }

  size_t HeapInfoTag(HeapInfo *) override;
  std::optional<size_t> Alloc(int, SYS_CLASS) override;

  explicit Mapped(system::System &, size_t);

private: /* image */
  struct CoreImage {
    int img_size; /* size of entire image */
    int hdr_size; /* size of the header */
#if 0
    std::string filename_; /* mapped file */
    int hp_pagesz_;        /* page size for this heap */
    int hp_npages_;        /* number of pages in the heap */
    int hp_uaddr_;         /* user virtual address */
    int hp_alloc_;         /* alloc barrier */
#endif
  };

public: /* iterator */
  /** * heapinfo iterator **/
  struct heapinfo_iter {
    typedef HeapInfo *iterator;

    Mapped *heap;
    iterator heapinfo;
    iterator current;

    heapinfo_iter(Mapped *heap)
        // NOLINTNEXTLINE (performance-no-int-to-ptr)
        : heap(heap), heapinfo(reinterpret_cast<HeapInfo *>(heap->heap_addr)),
          // NOLINTNEXTLINE (performance-no-int-to-ptr)
          current(reinterpret_cast<HeapInfo *>(heap->heap_addr)) {}

    iterator begin() { return heapinfo; }
    iterator end() { return nullptr; }

    bool operator!=(const struct heapinfo_iter &iter) {
      return current != iter.current;
    }

    iterator &operator++() { /* ++operator */
      current += Size(*current) / sizeof(HeapInfo);
      // NOLINTNEXTLINE (performance-no-int-to-ptr)
      if (current >= reinterpret_cast<HeapInfo *>(heap->alloc_barrier))
        current = nullptr;

      return *&current;
    }

    iterator operator++(int) {
      iterator retn = current;
      operator++();

      return retn;
    }
  } /* heapinfo_iter */;

}; /* class Mapped */

} /* namespace heap */
} /* namespace libmu */

#endif /* LIBMU_HEAP_MAPPED_H_ */
