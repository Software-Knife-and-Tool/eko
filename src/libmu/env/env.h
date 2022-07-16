/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  env.h: library functions
 **
 **/
#if !defined(LIBMU_ENV_ENV_H_)
#define LIBMU_ENV_ENV_H_

#include "libmu/core/context.h"
#include "libmu/core/env.h"

namespace libmu {

using Context = core::Context;
using Frame = Context::Frame;

namespace env {

void BindContext(Context &, Frame &);
void ContextInfo(Context &, Frame &);
void ContextFix(Context &, Frame &);
void EnvInfo(Context &, Frame &);
void MapNamespace(Context &, Frame &);
void FrameGet(Context &, Frame &);
void FramePop(Context &, Frame &);
void FramePush(Context &, Frame &);
void FrameRef(Context &, Frame &);
void FrameSetVar(Context &, Frame &);
void FrameSet(Context &, Frame &);
void Gc(Context &, Frame &);
void GetContext(Context &, Frame &);
void HeapInfo(Context &, Frame &);
void Namespace(Context &, Frame &);
void ResumeContext(Context &, Frame &);
void SaveImage(Context &, Frame &);
void SuspendContext(Context &, Frame &);

} /* namespace env */
} /* namespace libmu */

#endif /* LIBMU_ENV_ENV_H_ */
