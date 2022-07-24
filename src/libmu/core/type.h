/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  type.h: library tagged pointers
 **
 **/
#if !defined(LIBMU_CORE_TYPE_H_)
#define LIBMU_CORE_TYPE_H_

#include <cassert>
#include <cinttypes>
#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace libmu {
namespace core {

struct Env;

/** * pure abstract base class for types **/
class Type {
public:
  enum class Tag : uint64_t {};

public: /* Tag */
  /** * 2 bit staged low tag **/
  enum class TAG : uint8_t {
    DIRECT = 0,  /* 00 direct */
    UNUSED = 1,  /* 01 unused */
    CONS = 2,    /* 10 cons immediate */
    INDIRECT = 3 /* 11 indirect */
  };

  /** * extract tag **/
  static inline TAG TagOf(Tag ptr) {
    return static_cast<TAG>(std::to_underlying(ptr) & 0x3);
  }

  /** * make tagged pointer (fixnum uses this) **/
  static Tag Entag(uint64_t data, TAG tag) {
    return Tag(data | std::to_underlying(tag));
  }

  /** * tagged pointer of (void *) **/
  static Tag Entag(void *caddr, TAG tag) {
    return Tag(reinterpret_cast<uint64_t>(caddr) | std::to_underlying(tag));
  }

  /** * extended direct, 3 bits **/
  enum class DIRECT_CLASS : uint8_t {
    FIXNUM = 0, /* fixnums */
    CHAR = 1,   /* chars and syntax */
    VECTOR = 2, /* short tring vector */
    SYMBOL = 3, /* keyword symbol */
    FLOAT = 4,  /* single precision float */
    /** * room for a pony */
  };

  static const int DIRECT_STR_MAX = 7;

  /** * direct pointer layout: [d...].llltttTT **/
  static Tag MakeDirect(uint64_t data, size_t len, DIRECT_CLASS tag) {
    return Tag(((data << 8) | ((len & 0x7) << 5) |
                ((std::to_underlying(tag) & 0x7) << 2) |
                (std::to_underlying(TAG::DIRECT) & 0x3)));
  }

  /** * direct data (56 bits) **/
  static uint64_t DirectData(Tag ptr) { return std::to_underlying(ptr) >> 8; }

  /** * direct size (3 bits) **/
  static size_t DirectSize(Tag ptr) {
    return size_t{(std::to_underlying(ptr) >> 5) & 0x7};
  }

  /** * direct class (3 bits) **/
  static inline DIRECT_CLASS DirectClass(Tag ptr) {
    return DIRECT_CLASS((std::to_underlying(ptr) >> 2) & 0x7);
  }

  /** * constant symbols **/
  static const Tag T =
      Tag(('t' << 8) | ((1 & 0x7) << 5) |
          ((static_cast<uint8_t>(DIRECT_CLASS::SYMBOL) & 0x7) << 2) |
          (static_cast<uint8_t>(TAG::DIRECT) & 0x3));

  /** * constant symbol NIL **/
  static const Tag NIL =
      Tag(((('l' << 16) | ('i' << 8) | 'n') << 8) | ((3 & 0x7) << 5) |
          ((static_cast<uint8_t>(DIRECT_CLASS::SYMBOL) & 0x7) << 2) |
          (static_cast<uint8_t>(TAG::DIRECT) & 0x3));

  /** * constant symbol UNBOUND **/
  static const Tag UNBOUND = /* ASCII NUL */
      Tag((0x0 << 8) | ((0 & 0x7) << 5) |
          ((static_cast<uint8_t>(DIRECT_CLASS::SYMBOL) & 0x7) << 2) |
          (static_cast<uint8_t>(TAG::DIRECT) & 0x3));

  /** * system classes **/
  enum class SYS_CLASS : uint8_t {
    FIXNUM,
    CHAR,
    VECTOR,
    SYMBOL,
    FLOAT,
    BYTE,
    CONS,
    DOUBLE,
    EXCEPTION,
    FUNCTION,
    NAMESPACE,
    STREAM,
    T
  };

public: /* tag */
  static Tag View(Env &, Tag);
  static SYS_CLASS MapSymbolClass(Tag);
  static SYS_CLASS TypeOf(Env &, Tag);
  static Tag MapClassSymbol(SYS_CLASS);
  static bool IsClassSymbol(Tag);

  static inline Tag Bool(bool test) { return test ? T : NIL; }
  static inline bool IsDirect(Tag ptr) { return TagOf(ptr) == TAG::DIRECT; }
  static inline bool IsIndirect(Tag ptr) { return TagOf(ptr) == TAG::INDIRECT; }
  static inline bool Eq(Tag p0, Tag p1) { return p0 == p1; }
  static inline bool Null(Tag ptr) { return Eq(ptr, NIL); }

public:                        /* derived types */
  virtual Tag Heap(Env &) = 0; /* heap discipline */
  virtual ~Type() = default;

}; /* class Type */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_CORE_TYPE_H_ */
