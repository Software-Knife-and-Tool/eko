/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  env.h: global environment
 **
 **/
#if !defined(LIBMU_CORE_ENV_H_)
#define LIBMU_CORE_ENV_H_

#include <cassert>
#include <memory>
#include <stack>
#include <unordered_map>

#include "libmu/core/compile.h"
#include "libmu/core/context.h"
#include "libmu/core/heap.h"
#include "libmu/core/nscache.h"
#include "libmu/core/readtable.h"
#include "libmu/core/type.h"

#include "libmu/type/fixnum.h"
#include "libmu/type/namespace.h"

#include "libmu/system/internals.h"
#include "libmu/system/stream.h"

namespace libmu {
namespace core {

using Tag = Type::Tag;
using SYS_CLASS = Type::SYS_CLASS;

/** * Environment Class **/
struct Env {

public:
  static constexpr auto VERSION = "0.0.7";

public: /* mu function definition */
  typedef std::function<void(Context &, Context::Frame &)> MuFunction;
  typedef std::unordered_map<Tag, MuFunction> FnCache;

  struct MuFunctionDef {
    const char *name;
    MuFunction fn;
    int nreqs;
  };

  /** * read **/
  static Tag Read(Env &, Tag);
  static bool ReadWSUntilEof(Env &env, Tag stream);

  /** * eval **/
  static Tag Eval(Context &, Tag);
  static Tag Funcall(Context &, Tag, const std::vector<Tag> &);
  // static Tag StateMachine(Context &, Tag);

  /** * print **/
  static void Write(Env &, const std::string &, Tag, bool);
  static void Write(Env &, Tag, Tag, bool);

  /** * namespaces **/
  std::optional<Tag> MapNamespace(const std::string &) const;
  void AddNamespace(Tag);

  /** * garbage collection **/
  static bool IsGcMarked(Env &env, Tag ptr) {
    return env.heap->IsGcMarked(env, ptr);
  }

  static void GcMark(Env &env, Tag ptr) { env.heap->GcMark(env, ptr); }

  static void Gc(Env &, Tag);

public:
  std::unique_ptr<Compile> compiler;    /* compiler env */
  std::unique_ptr<ReadTable> readtable; /* readtable */
  std::unique_ptr<Heap> heap;           /* heap */

  std::map<std::string, Tag> namespaces; /* namespace map */

  std::unique_ptr<FnCache> fn_cache; /* native function cache */
  std::unique_ptr<NSCache> ns_cache; /* namespace cache */

  Tag version; /* version string */

  Tag env;        /* env namespace */
  Tag mu;         /* mu namespace */
  Tag uninterned; /* uninterned namespace */
  Tag system;     /* system namespace */

  Tag standard_input;  /* standard-input */
  Tag standard_output; /* standard-output */
  Tag error_output;    /* error-output */

  system::System &sys; /* system interface */

public: /* object */
  explicit Env(system::System &);

}; /* struct Env */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_CORE_ENV_H_ */
