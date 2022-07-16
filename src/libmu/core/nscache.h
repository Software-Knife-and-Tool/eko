/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  nscache.h: library symbol caches
 **
 **/
#if !defined(LIBMU_CORE_NSCACHE_H_)
#define LIBMU_CORE_NSCACHE_H_

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "libmu/core/env.h"
#include "libmu/core/heap.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/namespace.h"
#include "libmu/type/stream.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

namespace libmu {
namespace core {

struct NSCache {
private:
  /** * ns map **/
  typedef std::unordered_map<Tag, Tag> symbol_cache;
  typedef symbol_cache::const_iterator symbol_iter;

  /** * ns map, keyed by symbol name tag **/
  struct NSMap {
    symbol_cache externs;
    symbol_cache interns;
  };

  /** * ns cache, keyed by namespace tag **/
  std::unordered_map<Tag, std::unique_ptr<NSMap>> ns_cache_;

public:
  void add_ns_cache(Tag ns) { ns_cache_[ns] = std::make_unique<NSMap>(); }

  // NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
  std::optional<Tag> find_in_interns(Tag ns, Tag hash) {
    symbol_cache &cache = ns_cache_[ns]->interns;
    auto entry = cache.find(hash);

    if (entry == cache.end())
      return std::nullopt;

    const auto [name, symbol] = *entry;
    return symbol;
  }

  // NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
  std::optional<Tag> find_in_externs(Tag ns, Tag hash) {
    symbol_cache &cache = ns_cache_[ns]->externs;
    auto entry = cache.find(hash);

    if (entry == cache.end())
      return std::nullopt;

    const auto [name, symbol] = *entry;
    return symbol;
  }

  // NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
  void add_interns(Tag ns, Tag hash, Tag symbol) {

    if (ns_cache_[ns]->interns.contains(hash))
      throw std::runtime_error("hash collision");

    ns_cache_[ns]->interns.insert({hash, symbol});
  }

  // NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
  void add_externs(Tag ns, Tag hash, Tag symbol) {

    if (ns_cache_[ns]->externs.contains(hash))
      throw std::runtime_error("hash collision");

    ns_cache_[ns]->externs.insert({hash, symbol});
  }

}; /* struct NSCache */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_CORE_NSCACHE_H_ */
