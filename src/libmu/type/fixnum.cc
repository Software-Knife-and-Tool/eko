/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  fixnum.cc: library fixnums
 **
 **/
#include "libmu/type/fixnum.h"

#include <bitset>
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <utility>

#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/stream.h"

namespace libmu {
namespace type {

/** * print fixnum to stream **/
void Fixnum::Write(Env &env, Tag fixnum, Tag stream, bool) {
  assert(IsType(fixnum));
  assert(Stream::IsType(env, stream));

  std::ostringstream str;

  str << std::setbase(10) << Int64Of(fixnum);
  Env::Write(env, str.str(), stream, false);
}

} /* namespace type */
} /* namespace libmu */
