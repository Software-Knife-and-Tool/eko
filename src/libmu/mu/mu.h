/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu.h: library functions
 **
 **/
#if !defined(LIBMU_MU_MU_H_)
#define LIBMU_MU_MU_H_

#include "libmu/core/context.h"
#include "libmu/core/env.h"

namespace libmu {
namespace mu {

using Context = core::Context;
using Frame = Context::Frame;

void Car(Context &, Frame &);
void Cdr(Context &, Frame &);
void CharCode(Context &, Frame &);
void Close(Context &, Frame &);
void CodeChar(Context &, Frame &);
void Compile(Context &, Frame &);
void Cons(Context &, Frame &);
void Eq(Context &, Frame &);
void Eval(Context &, Frame &);
void Exception(Context &, Frame &);
void MapNamespace(Context &, Frame &);
void FixAdd(Context &, Frame &);
void FixLessThan(Context &, Frame &);
void FixMul(Context &, Frame &);
void FixSub(Context &, Frame &);
void FloatAdd(Context &, Frame &);
void FloatDiv(Context &, Frame &);
void FloatLessThan(Context &, Frame &);
void FloatMul(Context &, Frame &);
void FloatSub(Context &, Frame &);
void Floor(Context &, Frame &);
void Funcall(Context &, Frame &);
void FunctionInt(Context &ctx, Frame &fp);
void FunctionArity(Context &, Frame &);
void FunctionClone(Context &, Frame &);
void FunctionExtension(Context &, Frame &);
void FunctionForm(Context &, Frame &);
void FunctionFrameId(Context &, Frame &);
void Gc(Context &, Frame &);
void GetStringStream(Context &, Frame &);
void ImportOfNamespace(Context &, Frame &);
void Intern(Context &, Frame &);
void InternValue(Context &, Frame &);
void IsBound(Context &, Frame &);
void IsEof(Context &, Frame &);
void IsKeyword(Context &, Frame &);
void Keyword(Context &, Frame &);
void Letq(Context &, Frame &);
void ListLength(Context &, Frame &);
void Logand(Context &, Frame &);
void Logor(Context &, Frame &);
void MakeNamespace(Context &, Frame &);
void NameOfNamespace(Context &, Frame &);
void NamespaceSymbols(Context &, Frame &);
void Nth(Context &, Frame &);
void Nthcdr(Context &, Frame &);
void OpenStream(Context &, Frame &);
void Raise(Context &, Frame &);
void RaiseException(Context &, Frame &);
void Read(Context &, Frame &);
void ReadByte(Context &, Frame &);
void ReadChar(Context &, Frame &);
void SymbolName(Context &, Frame &);
void SymbolNamespace(Context &, Frame &);
void SymbolValue(Context &, Frame &);
void Truncate(Context &, Frame &);
void TypeOf(Context &, Frame &);
void UnReadByte(Context &, Frame &);
void UnReadChar(Context &, Frame &);
void UninternedSymbol(Context &, Frame &);
void VectorCons(Context &, Frame &);
void VectorLength(Context &, Frame &);
void VectorRef(Context &, Frame &);
void VectorSlice(Context &, Frame &);
void VectorType(Context &, Frame &);
void View(Context &, Frame &);
void WithException(Context &, Frame &);
void Write(Context &, Frame &);
void WriteByte(Context &, Frame &);
void WriteChar(Context &, Frame &);

} /* namespace mu */
} /* namespace libmu */

#endif /* LIBMU_MU_MU_H_ */
