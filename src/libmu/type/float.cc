/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  float.cc: library floats
 **
 **/
#include "libmu/type/float.h"

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

/** * print float object to stream **/
void Float::Write(Env &env, Tag lfloat, Tag stream, bool) {
  assert(IsType(lfloat));
  assert(Stream::IsType(env, stream));

  std::ostringstream str;

  str << std::fixed << std::setprecision(6) << Float::FloatOf(lfloat);
  Env::Write(env, str.str(), stream, false);
}

/** * print double object to stream **/
void Double::Write(Env &env, Tag dbl, Tag stream, bool) {
  assert(IsType(env, dbl));
  assert(Stream::IsType(env, stream));

  std::ostringstream str;

  str << std::fixed << std::setprecision(6) << Double::DoubleOf(env, dbl);
  Env::Write(env, str.str(), stream, false);
}

} /* namespace type */
} /* namespace libmu */
