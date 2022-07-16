/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  libmu.cc: library API
 **
 **/
#include "libmu/libmu.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>

#include <unistd.h>

#include "libmu/core/context.h"

#include "libmu/core/env.h"
#include "libmu/core/type.h"

#include "libmu/type/exception.h"
#include "libmu/type/namespace.h"
#include "libmu/type/stream.h"
#include "libmu/type/symbol.h"
#include "libmu/type/vector.h"

#include "libmu/system/internals.h"

namespace libmu {

using Symbol = type::Symbol;
using Stream = type::Stream;
using Exception = type::Exception;
using Context = core::Context;

namespace api {

/** * constants interface **/
uint64_t mu_t() { return std::to_underlying(Type::T); }
uint64_t mu_nil() { return std::to_underlying(Type::NIL); }

/** * version **/
const char *mu_version() { return Env::VERSION; }

/** * read interface **/
uint64_t mu_read_stream(void *ctx, uint64_t stream) {
  auto ctxt = reinterpret_cast<Context *>(ctx);

  if (stream == mu_t())
    stream =
        std::to_underlying(Symbol::value(ctxt->env, ctxt->env.standard_input));

  if (!Stream::IsType(ctxt->env, Tag(stream))) {
    throw std::runtime_error("stream type\n");
  }

  return std::to_underlying(core::Env::Read(ctxt->env, Type::Tag(stream)));
}

/** * eof on stream? **/
bool mu_eof(void *ctx, uint64_t stream) {
  auto ctxt = reinterpret_cast<Context *>(ctx);

  if (stream == mu_t())
    stream =
        std::to_underlying(Symbol::value(ctxt->env, ctxt->env.standard_input));

  return Stream::IsEof(ctxt->env, Type::Tag(stream));
}

/** * read interface **/
uint64_t mu_read_cstr(void *ctx, const char *src) {
  auto ctxt = reinterpret_cast<Context *>(ctx);

  return std::to_underlying(
      core::Env::Read(ctxt->env, Stream::OpenInputString(ctxt->env, src)));
}

/** * princ/prin1 interface **/
void mu_write(void *ctx, uint64_t ptr, uint64_t stream, bool esc) {
  auto ctxt = reinterpret_cast<Context *>(ctx);

  if (stream == mu_nil())
    stream =
        std::to_underlying(Symbol::value(ctxt->env, ctxt->env.standard_output));

  core::Env::Write(ctxt->env, Type::Tag(ptr), Type::Tag(stream), esc);
}

void mu_writeln(void *ctx, uint64_t ptr, uint64_t stream, bool esc) {
  auto ctxt = reinterpret_cast<Context *>(ctx);

  if (stream == mu_nil())
    stream =
        std::to_underlying(Symbol::value(ctxt->env, ctxt->env.standard_output));

  core::Env::Write(ctxt->env, Type::Tag(ptr), Type::Tag(stream), esc);
  core::Env::Write(ctxt->env, "\n", Type::Tag(stream), false);
}

/** * catch library throws **/
bool mu_with_exception(void *ctx, bool throwp,
                       const std::function<void(void *)> &fn) {
  auto ctxt = reinterpret_cast<Context *>(ctx);

  try {
    fn(ctxt);
  } catch (Type::Tag &exception) {
    auto std_error = Symbol::value(ctxt->env, ctxt->env.error_output);

    auto tag = Exception::tag(ctxt->env, exception);
    auto eclass = Exception::eclass(ctxt->env, exception);
    auto etype = Exception::etype(ctxt->env, exception);
    auto source = Exception::source(ctxt->env, exception);

    core::Env::Write(ctxt->env, "uncaught libmu exception: ", std_error, false);
    core::Env::Write(ctxt->env, tag, std_error, true);
    core::Env::Write(ctxt->env, " ", std_error, false);
    core::Env::Write(ctxt->env, eclass, std_error, true);
    core::Env::Write(ctxt->env, " ", std_error, false);
    core::Env::Write(ctxt->env, etype, std_error, false);
    core::Env::Write(ctxt->env, ", ", std_error, false);
    core::Env::Write(ctxt->env, source, std_error, true);
    core::Env::Write(ctxt->env, "\n", std_error, false);

    if (throwp)
      throw std::runtime_error("exception throw from libmu");

    return false;
  } catch (const std::exception &except) {
    std::cerr << except.what() << std::endl;

    if (throwp)
      throw std::runtime_error("unexpected throw from libmu");

    exit(EXIT_FAILURE);
  }

  return true;
}

/** * eval **/
uint64_t mu_eval(void *ctx, uint64_t form) {
  Context &context = *static_cast<Context *>(ctx);

  return std::to_underlying(core::Env::Eval(
      context, core::Compile::Form(context, static_cast<Type::Tag>(form))));
}

/** * main thread context **/
void *mu_context(int sin, int sout, int serr, char **, int) {
  auto system = new system::System(sin, sout, serr);
  Env *env = new Env(*system);

  return static_cast<void *>(new core::Context(*env, Type::NIL));
}

} /* namespace api */
} /* namespace libmu */
