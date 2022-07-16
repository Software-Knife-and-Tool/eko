/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  compile.h: library compiler environment
 **
 **/
#if !defined(LIBMU_CORE_COMPILE_H_)
#define LIBMU_CORE_COMPILE_H_

#include <cassert>
#include <condition_variable>
#include <csetjmp>
#include <memory>
#include <mutex>
#include <optional>
#include <stack>
#include <thread>
#include <unordered_map>

#include "libmu/core/context.h"
#include "libmu/core/type.h"

namespace libmu {
namespace core {

using Tag = Type::Tag;

struct Compile {
public:
  static Tag Form(Context &, Tag);

  Tag frameid();

public:
  int frame_id = 1;          /* lexical binding id */
  std::vector<Tag> lexicals; /* lexical symbols */
};                           /* struct Compile */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_CORE_COMPILE_H_ */
