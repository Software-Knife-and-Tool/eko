/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  context.h: library environment
 **
 **/
#if !defined(LIBMU_CORE_CONTEXT_H_)
#define LIBMU_CORE_CONTEXT_H_

#include <cassert>
#include <condition_variable>
#include <csetjmp>
#include <memory>
#include <mutex>
#include <optional>
#include <stack>
#include <thread>
#include <unordered_map>

#include "libmu/core/type.h"

#include "libmu/type/fixnum.h"

namespace libmu {
namespace core {

using Tag = Type::Tag;

/** * contexts are a collection of frames **/
struct Context {
public:
  struct Frame {
    Tag func;  /* applied function */
    Tag *argv; /* argument list */
    Tag value; /* return value */

    Frame(Tag func, Tag *argv) : func(func), argv(argv), value(Type::UNBOUND) {}
  };

  struct StackInfo {
    size_t max;     /* maximum depth */
    size_t calls;   /* number of inserts */
    size_t current; /* total depth */
  };

  void DynamicContextPush(Frame &fr) {

    dynamic.push_back(&fr);
    stack_info.calls++;
    if (dynamic.size() > stack_info.max)
      stack_info.max = dynamic.size();

    stack_info.current = dynamic.size();
  }

  void DynamicContextPop() {

    dynamic.pop_back();
    stack_info.current = dynamic.size();
  }

  std::optional<Frame *> MapDynamicContext(size_t offset) {

    /* que? */
    if (dynamic.size() == 0 || offset >= dynamic.size())
      return std::nullopt;

    return dynamic.at(dynamic.size() - (1 + offset));
  }

  void LexicalContextPush(Tag id, Frame &fr) { lexical[id].push_back(&fr); }

  void LexicalContextPop(Tag id) { lexical[id].pop_back(); }

  std::optional<Frame *> MapLexicalContext(Tag id, size_t offset) {
    size_t size = lexical[id].size();

    if (size == 0 || offset >= size)
      return std::nullopt;

    return lexical[id].at(lexical[id].size() - (offset + 1));
  }

  void Suspend() {

    std::unique_lock lk(cv_mutex);
    cv_resume.wait(lk);
  }

  static void Resume(Context *ctx) {

    std::unique_lock lk(ctx->cv_mutex);
    ctx->cv_resume.notify_one();
  }

  Tag FrameToTag(Frame &);
  Tag FuncIdOf(Tag);
  Tag NthArgOf(Tag, size_t);
  void SetNthArgOf(Tag, Tag, size_t);
  Frame *TagToFrame(Tag);
  Tag CaptureContext();

  void GcFrame(Env &, Frame &);
  void GcContext();

public:
  /* global environment */
  Env &env;

  /* execution environment */
  std::unique_ptr<std::thread> thread;

  /* locks */
  std::mutex cv_mutex;
  std::condition_variable cv_resume;

  /* dynamic environment */
  std::vector<Frame *> dynamic;                           /* control stack */
  std::unordered_map<Tag, std::vector<Frame *>> lexical;  /* lexical bindings */
  std::unordered_map<Tag, std::vector<Frame *>> closures; /* closure bindings */

  StackInfo stack_info{0, 0, 0}; /* control stack monitoring */

public:
  explicit Context(Env &, Tag);
}; /* struct Context */

} /* namespace core */
} /* namespace libmu */

#endif /* LIBMU_CORE_CONTEXT_H_ */
