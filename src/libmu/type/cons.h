/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  cons.h: library conses
 **
 **/
#if !defined(LIBMU_TYPE_CONS_H_)
#define LIBMU_TYPE_CONS_H_

#include <cassert>
#include <functional>
#include <memory>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

namespace libmu {

using Env = core::Env;
using Tag = core::Type::Tag;
using Type = core::Type;
using Heap = core::Heap;

namespace type {

/** * cons type class **/
class Cons : public Type {
public:
  struct Layout {
    Tag car;
    Tag cdr;
  };

  Layout cons_;

public: /* tag */
  static Tag car(Env &, Tag);
  static Tag cdr(Env &, Tag);

  static bool IsType(Env &, Tag);
  static bool IsList(Env &, Tag);

  static Tag List(Env &, const std::vector<Tag> &);
  static Tag ListDot(Env &, const std::vector<Tag> &);

  static Tag Nth(Env &, Tag, size_t);
  static size_t Length(Env &, Tag);

  static void Write(Env &, Tag, Tag, bool);
  static Tag Read(Env &, Tag);

  static void Gc(Env &, Tag);

public: /* type model */
  Tag Heap(Env &) override;

public:
  // NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
  explicit Cons(Tag car, Tag cdr) : Type() {
    cons_.car = car;
    cons_.cdr = cdr;
  }

  ~Cons() override = default;

public: /** * cons iterator **/
  struct iter {
    typedef Tag iterator;

    iterator cons_;
    iterator current_;
    Env &env_;

    iter(Env &env, Tag cons)
        : cons_(Null(cons) ? end() : cons), current_(cons_), env_(env) {
      assert(IsList(env, cons));
    }

    iterator begin() { return cons_; }
    iterator end() { return Type::NIL; }

    bool operator!=(const struct iter &iter) {
      return current_ != iter.current_;
    }

    iterator &operator++() { /* ++operator */

      /** * this cleverness coutesy of dotted pairs **/
      Tag cdr = Cons::cdr(env_, current_);
      current_ = IsType(env_, cdr) ? cdr : Type::NIL;

      return *&current_;
    }

    iterator operator++(int) { /* operator++ */

      iterator retn = current_;
      operator++();

      return retn;
    }
  };

}; /* class Cons */

} /* namespace type */
} /* namespace libmu */

#endif /* LIBMU_TYPE_CONS_H_ */
