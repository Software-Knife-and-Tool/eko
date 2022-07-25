/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  heap.h: heap parent class
 **
 **/
#if !defined(LIBMU_CORE_HEAP_H_)
#define LIBMU_CORE_HEAP_H_

#include <cassert>
#include <cinttypes>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "libmu/core/type.h"
#include "libmu/system/internals.h"

namespace libmu {
namespace core {

/** * heap pure abstract base class **/
class Heap {
public:
  /** * tagged pointer to layout pointer **/
  static void *LayoutAddr(Env &env, Type::Tag);

  template <typename T> static inline T *Layout(Env &env, Type::Tag ptr) {
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    return reinterpret_cast<T *>(LayoutAddr(env, ptr));
  }

  /** * is this a heap tag? **/
  static bool IsHeapTag(Type::Tag ptr) { return std::to_underlying(ptr) >> 63; }

public:
  /** * HeapInfo:
   **   reloc offset (uint32_t),
   **   size (uint16_t),
   **   ref bits (uint8_t),
   **   tag (uint8_t)
   **/
  enum class HeapInfo : uint64_t {};

  /** * bytes to heap words **/
  static size_t HeapWords(size_t nbytes) {
    return (nbytes + sizeof(uint64_t) - 1) / sizeof(uint64_t);
  }

  static HeapInfo MakeHeapInfo(size_t size, Type::SYS_CLASS tag) {
    return HeapInfo((HeapWords(size) << 16) | std::to_underlying(tag));
  }

  /** * SYS_CLASS from HeapInfo **/
  static Type::SYS_CLASS SysClass(HeapInfo hinfo) {
    return Type::SYS_CLASS(std::to_underlying(hinfo) & 0xff);
  }

  /** * get/set ref bits **/
  static uint8_t RefBits(HeapInfo hinfo) {
    return (std::to_underlying(hinfo) >> 8) & 0xff;
  }

  static HeapInfo RefBits(HeapInfo hinfo, uint8_t refbits) {
    auto hi = std::to_underlying(hinfo);
    return HeapInfo{(hi & ~(uint64_t{0xff} << 8)) | refbits << 8};
  }

  /** * get heap object size **/
  static size_t Size(HeapInfo hinfo) {
    return sizeof(uint64_t) * ((std::to_underlying(hinfo) >> 16) & 0xffff);
  }

  /** * set heap object size **/
  static HeapInfo Size(HeapInfo hinfo, uint32_t size) {
    return HeapInfo((std::to_underlying(hinfo) & ~(uint64_t{0xffff} << 16)) |
                    ((HeapWords(size) & 0xffff) << 16));
  }

  /** * get reloc **/
  static uint64_t Reloc(HeapInfo hinfo) {
    return sizeof(uint64_t) * ((std::to_underlying(hinfo) >> 32) & 0xffffffff);
  }

  /** * set reloc **/
  static HeapInfo Reloc(HeapInfo hinfo, uint64_t reloc) {
    return HeapInfo(
        (std::to_underlying(hinfo) & ~(uint64_t{0xffffffff} << 32)) |
        (((reloc / 8) & 0xffffffff) << 32));
  }

public: /* gc */
  std::unordered_map<Type::SYS_CLASS, std::vector<HeapInfo *>> free_lists;
  size_t nfree;

  static void Gc(Env &);
  static void GcMark(Env &, Type::Tag);
  static bool IsGcMarked(Env &, Type::Tag);
  static void GcSweep(Env &);

  static std::optional<size_t> GcAlloc(Env &, int, Type::SYS_CLASS);

public: /* derived types */
  virtual HeapInfo *Map(Type::Tag) = 0;
  virtual void Map(std::function<void(HeapInfo *)>) = 0;

  virtual void *HeapAddr(size_t) = 0;
  virtual size_t HeapInfoTag(HeapInfo *hp) = 0;
  virtual std::optional<size_t> Alloc(int, Type::SYS_CLASS) = 0;
  virtual std::optional<Type::Tag> MapString(std::string &) = 0;

  virtual Type::Tag InternString(Env &, Type::Tag) = 0;

  virtual size_t HeapSize() = 0;
  virtual size_t HeapAlloc() = 0;

  virtual size_t TypeAlloc(Type::SYS_CLASS) = 0;
  virtual size_t TypeAlloc() = 0;
  virtual size_t Room(Type::SYS_CLASS) = 0;
  virtual size_t Room() = 0;

  explicit Heap() = default;
  virtual ~Heap() = default;

}; /* class Heap */

} // namespace core
} /* namespace libmu */

#endif /* LIBMU_CORE_HEAP_H_ */
