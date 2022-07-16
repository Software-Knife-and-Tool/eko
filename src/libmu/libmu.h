/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  libmu.h: library API
 **
 **/
#if !defined(LIBMU_LIBMU_H_)
#define LIBMU_LIBMU_H_

#include <cassert>
#include <cinttypes>
#include <functional>
#include <memory>

namespace libmu {
namespace api {

extern "C" {
uint64_t mu_t();
uint64_t mu_nil();
const char *mu_version();
bool mu_eof(void *, uint64_t);
uint64_t mu_eval(void *, uint64_t);
uint64_t mu_read_stream(void *, uint64_t);
uint64_t mu_read_cstr(void *, const char *);
void mu_write(void *, uint64_t, uint64_t, bool);
void mu_writeln(void *, uint64_t, uint64_t, bool);
bool mu_with_exception(void *, bool, const std::function<void(void *)> &);
void *mu_context(int, int, int, char *argv[], int);
}

} /* namespace api */
} /* namespace libmu */

#endif /* LIBMU_LIBMU_H_ */
