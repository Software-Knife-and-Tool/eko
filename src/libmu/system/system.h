/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  system.h: library functions
 **
 **/
#if !defined(LIBMU_SYSTEM_SYSTEM_H_)
#define LIBMU_SYSTEM_SYSTEM_H_

#include "libmu/core/context.h"
#include "libmu/core/env.h"

namespace libmu {

using Context = core::Context;
using Frame = Context::Frame;

namespace system {

void Errno(Context &, Frame &);
void Cwd(Context &, Frame &);
void Logname(Context &, Frame &);
void Chdir(Context &, Frame &);
[[noreturn]] void Exit(Context &, Frame &);
void SystemCommand(Context &, Frame &);
void Times(Context &, Frame &);
void ProcessEnv(Context &, Frame &d);

} /* namespace system */
} /* namespace libmu */

#endif /* LIBMU_MU_MU_H_ */
