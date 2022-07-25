/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  vector.h: library vectors
 **
 **/
#if !defined(LIBMU_TYPE_VECTOR_H_)
#define LIBMU_TYPE_VECTOR_H_

#include <cassert>
#include <cstring>
#include <functional>
#include <memory>
#include <vector>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/fixnum.h"

namespace libmu {

using Heap = core::Heap;

namespace type {

/** * Vector type class **/
class Vector : public Type {
public:
  struct Layout {
    Tag type;   /* vector type */
    Tag length; /* length of vector */
    Tag offset; /* fixnum of raw data */
  };

  Layout vector_;
  size_t sizeof_;

  Tag tag_;              /* for direct strings */
  std::string interned_; /* for string interning */

public: /* string */
  static std::string StdStringOf(Env &env, Tag str) {
    assert(IsType(str));

    return std::string{Data<char>(env, str), Length(env, str)};
  }

  static Tag MakeDirect(const std::string &str) {
    assert(str.size() <= DIRECT_STR_MAX);

    uint64_t buffer = 0;

    std::memcpy(reinterpret_cast<void *>(&buffer), str.data(), str.size());

    return Type::MakeDirect(buffer, str.size(), DIRECT_CLASS::VECTOR);
  }

  static Tag MakeDirect(uint8_t *offset, int nbytes) {
    assert(nbytes <= DIRECT_STR_MAX);

    uint64_t buffer = 0;

    std::memcpy(reinterpret_cast<void *>(&buffer), offset, nbytes);

    return Type::MakeDirect(buffer, nbytes, DIRECT_CLASS::VECTOR);
  }

public: /* Tag */
  static const int MAX_LENGTH = 1024;

  /** * accessors **/
  static SYS_CLASS type(Env &env, Tag vec) {
    return Type::MapSymbolClass(Heap::Layout<Layout>(env, vec)->type);
  }

  static int length(Env &env, Tag vec) {
    return Fixnum::Int64Of(Heap::Layout<Layout>(env, vec)->length);
  }

  static uint64_t offset(Env &env, Tag vec) {
    return Fixnum::Int64Of(Heap::Layout<Layout>(env, vec)->offset);
  }

  static Tag Map(Env &, Tag, Tag);
  static bool IsType(Tag);

  static inline bool IsTyped(Env &env, Tag ptr, SYS_CLASS vtype) {
    return (IsDirect(ptr) && (DirectClass(ptr) == DIRECT_CLASS::VECTOR) &&
            (vtype == SYS_CLASS::CHAR)) ||
           (IsType(ptr) && (type(env, ptr) == vtype));
  }

  static void *DataV(Env &, Tag &);

  template <typename T> static T *Data(Env &env, Tag &vector) {
    assert(IsType(vector));

    return IsDirect(vector)
               ? reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(&vector) + 1)
               : reinterpret_cast<T *>(DataV(env, vector));
  }

  template <typename T> static inline T Ref(Env &env, Tag &vector, int index) {
    assert(IsType(vector));

    return Data<T>(env, vector)[index];
  }

  static size_t inline Length(Env &env, Tag vec) {
    assert(IsType(vec));

    return (IsDirect(vec) && DirectClass(vec) == DIRECT_CLASS::VECTOR)
               ? DirectSize(vec)
               : Fixnum::Int64Of(Heap::Layout<Layout>(env, vec)->length);
  }

  static SYS_CLASS inline TypeOf(Env &env, Tag vec) {
    assert(IsType(vec));

    return (IsDirect(vec) && DirectClass(vec) == DIRECT_CLASS::VECTOR
                ? SYS_CLASS::CHAR
                : Type::MapSymbolClass(Heap::Layout<Layout>(env, vec)->type));
  }

  static inline Tag VecType(Env &env, Tag vec) {
    assert(IsType(vec));

    return Type::MapClassSymbol(TypeOf(env, vec));
  }

  static void Gc(Env &, Tag);
  static Tag Slice(Env &env, Tag, Tag, Tag);
  static Tag ListToVector(Env &, Tag, Tag);
  static void Write(Env &, Tag, Tag, bool);
  static Tag Read(Env &, Tag);
  static Tag ReadString(Env &, Tag);
  static Tag View(Env &, Tag);

public: /* type model */
  Tag Heap(Env &) override;

  explicit Vector() = default;
  explicit Vector(Tag){};

  explicit Vector(std::vector<Tag> src) {
    sizeof_ = sizeof(Tag);

    vector_.type = Type::MapClassSymbol(SYS_CLASS::T);
    vector_.length = Fixnum(src.size()).tag_;
    vector_.offset = Fixnum(reinterpret_cast<size_t>(src.data())).tag_;
  }

  explicit Vector(const std::string &src) {
    sizeof_ = sizeof(uint8_t);

    interned_ = src;

    vector_.type = Type::MapClassSymbol(SYS_CLASS::CHAR);
    vector_.length = Fixnum(src.size()).tag_;
    vector_.offset = Fixnum(reinterpret_cast<size_t>(src.data())).tag_;

    if (src.size() <= DIRECT_STR_MAX)
      tag_ = MakeDirect(src);
  }

  explicit Vector(std::vector<char> src) {
    sizeof_ = sizeof(char);

    vector_.type = Type::MapClassSymbol(SYS_CLASS::CHAR);
    vector_.length = Fixnum(src.size()).tag_;
    vector_.offset = Fixnum(reinterpret_cast<size_t>(src.data())).tag_;

    std::string ssrc = std::string(src.begin(), src.end());

    if (src.size() <= DIRECT_STR_MAX)
      tag_ = MakeDirect(std::string(ssrc));
  }

  explicit Vector(std::vector<float> src) {
    sizeof_ = sizeof(float);

    vector_.type = Type::MapClassSymbol(SYS_CLASS::FLOAT);
    vector_.length = Fixnum(src.size()).tag_;
    vector_.offset = Fixnum(reinterpret_cast<size_t>(src.data())).tag_;
  }

  explicit Vector(std::vector<double> src) {
    sizeof_ = sizeof(double);

    vector_.type = Type::MapClassSymbol(SYS_CLASS::DOUBLE);
    vector_.length = Fixnum(src.size()).tag_;
    vector_.offset = Fixnum(reinterpret_cast<size_t>(src.data())).tag_;
  }

  explicit Vector(std::vector<uint8_t> src) {
    sizeof_ = sizeof(uint8_t);

    vector_.type = Type::MapClassSymbol(SYS_CLASS::BYTE);
    vector_.length = Fixnum(src.size()).tag_;
    vector_.offset = Fixnum(reinterpret_cast<size_t>(src.data())).tag_;
  }

  explicit Vector(std::vector<int64_t> src) {
    sizeof_ = sizeof(int64_t);

    vector_.type = Type::MapClassSymbol(SYS_CLASS::FIXNUM);
    vector_.length = Fixnum(src.size()).tag_;
    vector_.offset = Fixnum(reinterpret_cast<size_t>(src.data())).tag_;
  }

  ~Vector() override = default;

public: /** * vector iterator **/
  template <typename T> struct iter {
    typedef T *iterator;

    iterator vector;
    std::ptrdiff_t length;
    iterator current;

    /* because the vector might be a direct, we need to ref it */
    /* fix: figure out how to const the ref */
    iter(Env &env, Tag &vec)
        : vector(Data<T>(env, vec)),
          length(static_cast<std::ptrdiff_t>(Length(env, vec))),
          current(begin()) {}

    iterator begin() { return length ? vector : end(); }
    iterator end() { return nullptr; }

    iterator &operator++() { /* operator++ */
      std::ptrdiff_t curlen = current - vector;

      if (curlen < (length - 1))
        ++current;
      else
        current = end();

      return *&current;
    }

    iterator operator++(int) { /* ++operator */

      iterator retn = current;

      operator++();
      return retn;
    }

    T operator*() { return *current; }
  };

}; /* class Vector */

} /* namespace type */
} /* namespace libmu */

#endif /* LIBMU_TYPE_VECTOR_H_ */
