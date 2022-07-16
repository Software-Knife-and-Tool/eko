/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  sys-functions.cc: system functions
 **
 **/
#include <cassert>
#include <functional>
#include <iomanip>
#include <map>
#include <optional>
#include <vector>

#include "libmu/core/context.h"
#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/cons.h"
#include "libmu/type/exception.h"
#include "libmu/type/fixnum.h"
#include "libmu/type/float.h"
#include "libmu/type/function.h"
#include "libmu/type/stream.h"
#include "libmu/type/symbol.h"

#include "libmu/system/internals.h"

namespace libmu {

using Cons = type::Cons;
using Exception = type::Exception;
using Fixnum = type::Fixnum;
using Float = type::Float;
using Function = type::Function;
using Symbol = type::Symbol;
using Vector = type::Vector;

using Context = core::Context;
using Env = core::Env;
using Frame = core::Context::Frame;
using Tag = core::Tag;
using Type = core::Type;

namespace system {

/** * (errno) **/
void Errno(Context &, Frame &fp) {
  fp.value = Fixnum(system::System::Errno()).tag_;
}

/** * (cwd) **/
void Cwd(Context &ctx, Frame &fp) {
  std::optional<const std::string> cwd_opt = system::System::Cwd();

  if (!cwd_opt.has_value())
    Exception::Raise(ctx.env, "cwd", "error", "system", Type::NIL);

  fp.value = Vector(cwd_opt.value()).Heap(ctx.env);
}

/** * (logname) **/
void Logname(Context &ctx, Frame &fp) {
  std::optional<const std::string> logname_opt = system::System::Logname();

  if (!logname_opt.has_value())
    Exception::Raise(ctx.env, "logname", "error", "system", Type::NIL);

  fp.value = Vector(logname_opt.value()).Heap(ctx.env);
}

/** * (chdir path) **/
void Chdir(Context &ctx, Frame &fp) {
  Tag path = fp.argv[0];

  if (!Vector::IsTyped(ctx.env, path, Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, "chdir", "error", "type", path);

  std::optional<const std::string> path_opt =
      system::System::Chdir(Vector::StdStringOf(ctx.env, path));

  if (!path_opt.has_value())
    Exception::Raise(ctx.env, "chdir", "error", "system", path);

  fp.value = path;
}

/** * (exit fixnum) never returns **/
[[noreturn]] void Exit(Context &ctx, Frame &fp) {
  Tag rc = fp.argv[0];

  if (!Fixnum::IsType(rc))
    Exception::Raise(ctx.env, "exit", "error", "type", rc);

  fp.value = rc;
  system::System::Exit(Fixnum::Int64Of(rc));
}

/** * (system string) **/
void SystemCommand(Context &ctx, Frame &fp) {
  Tag cmd = fp.argv[0];

  if (!Vector::IsTyped(ctx.env, cmd, Type::SYS_CLASS::CHAR))
    Exception::Raise(ctx.env, "system", "error", "type", cmd);

  std::optional<int> rc_opt =
      system::System::SysExec(Vector::StdStringOf(ctx.env, cmd));

  if (!rc_opt.has_value())
    Exception::Raise(ctx.env, "system", "error", "exec", cmd);

  fp.value = Fixnum(rc_opt.value()).tag_;
}

/** * (utimes) => vector **/
void Times(Context &ctx, Frame &fp) {
  std::optional<uint64_t> st = system::System::SystemTime();
  std::optional<uint64_t> rt = system::System::ProcessTime();

  if (!st.has_value() || !rt.has_value())
    Exception::Raise(ctx.env, "utimes", "error", "clock", Type::NIL);

  std::vector<Tag> view = std::vector<Tag>{Symbol::Keyword("clock"),
                                           Fixnum(size_t{st.value()}).tag_,
                                           Fixnum(size_t{rt.value()}).tag_};

  fp.value = Vector(view).Heap(ctx.env);
}

/** * (procenv => list **/
void ProcessEnv(Context &ctx, Frame &fp) {
  std::vector<std::pair<const std::string, const std::string>> env_vector =
      system::System::ProcessEnv();

  Env &env = ctx.env;
  std::vector<Tag> env_list{};
  std::transform(
      begin(env_vector), end(env_vector), back_inserter(env_list),
      [&env](std::pair<std::string, std::string> ep) {
        const auto [name, value] = ep;
        return Cons(Vector(name).Heap(env), Vector(value).Heap(env)).Heap(env);
      });

  fp.value = Cons::List(ctx.env, env_list);
}

} /* namespace system */
} /* namespace libmu */
