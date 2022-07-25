/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  mu-vector.cc: library vector functions
 **
 **/
#include <cassert>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/char.h"
#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/vector.h"

namespace libmu {

using Char = type::Char;
using Cons = type::Cons;
using Exception = type::Exception;
using Fixnum = type::Fixnum;
using Float = type::Float;
using Function = type::Function;
using ReadTable = core::ReadTable;
using Symbol = type::Symbol;
using Vector = type::Vector;

using Context = core::Context;
using Env = core::Env;
using Frame = core::Context::Frame;
using Tag = core::Tag;
using Type = core::Type;

namespace mu {

/** * (sv-ref vector) => object **/
void VectorRef(Context &ctx, Frame &fp) {
  Tag vector = fp.argv[0];
  Tag index = fp.argv[1];

  if (!Vector::IsType(vector))
    Exception::Raise(ctx.env, "sv-ref", "error", "vector", vector);

  if (!Fixnum::IsType(index))
    Exception::Raise(ctx.env, "sv-ref", "error", "index", index);

  if (Fixnum::Int64Of(index) < 0)
    Exception::Raise(ctx.env, "sv-ref", "error", "range", index);

  if (Vector::Length(ctx.env, vector) <=
      static_cast<size_t>(Fixnum::Int64Of(index)))
    Exception::Raise(ctx.env, "sv-ref", "error", "range", index);

  uint64_t nth = Fixnum::Int64Of(index);
  switch (Vector::TypeOf(ctx.env, vector)) {
  case Type::SYS_CLASS::FIXNUM:
    fp.value = Fixnum(Vector::Ref<int64_t>(ctx.env, vector, nth)).tag_;
    break;
  case Type::SYS_CLASS::BYTE:
    fp.value = Fixnum(Vector::Ref<uint8_t>(ctx.env, vector, nth)).tag_;
    break;
  case Type::SYS_CLASS::CHAR:
    fp.value = Char(Vector::Ref<uint8_t>(ctx.env, vector, nth)).tag_;
    break;
  case Type::SYS_CLASS::FLOAT:
    fp.value = Float(Vector::Ref<float>(ctx.env, vector, nth)).tag_;
    break;
  case Type::SYS_CLASS::T:
    fp.value = Vector::Ref<Type::Tag>(ctx.env, vector, nth);
    break;
  default:
    throw std::runtime_error("vector type botch");
  }
}

/** * (sv-len vector) => fixnum **/
void VectorLength(Context &ctx, Frame &fp) {
  Tag vector = fp.argv[0];

  if (!Vector::IsType(vector))
    Exception::Raise(ctx.env, "vc-len", "error", "type", vector);

  fp.value = Fixnum(Vector::Length(ctx.env, vector)).tag_;
}

/** * (slice vector offset len) => vector **/
void VectorSlice(Context &ctx, Frame &fp) {
  Tag vector = fp.argv[0];
  Tag offset = fp.argv[1];
  Tag len = fp.argv[2];

  if (!Vector::IsType(vector))
    Exception::Raise(ctx.env, "slice", "error", "type", vector);

  if (!Fixnum::IsType(offset))
    Exception::Raise(ctx.env, "slice", "error", "type", offset);

  if (!Fixnum::IsType(len))
    Exception::Raise(ctx.env, "slice", "error", "type", len);

  fp.value = Vector::Slice(ctx.env, vector, offset, len);
}

/** * (sv-type vector) => symbol **/
void VectorType(Context &ctx, Frame &fp) {
  if (!Vector::IsType(fp.argv[0]))
    Exception::Raise(ctx.env, "sv-type", "error", "type", fp.argv[0]);

  fp.value = Vector::VecType(ctx.env, fp.argv[0]);
}

/** * (list-sv type list) => vector **/
void VectorCons(Context &ctx, Frame &fp) {
  Tag type = fp.argv[0];
  Tag list = fp.argv[1];

  if (!Symbol::IsType(type))
    Exception::Raise(ctx.env, "list-sv", "error", "type", type);

  if (!Cons::IsList(list))
    Exception::Raise(ctx.env, "list-sv", "error", "type", list);

  fp.value = Vector::ListToVector(ctx.env, type, list);
}

} /* namespace mu */
} /* namespace libmu */
