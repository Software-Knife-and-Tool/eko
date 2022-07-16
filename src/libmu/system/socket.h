/********
 **
 **  SPDX-FileCopyrightText: Copyright 2017-2022 James M. Putnam
 **  SPDX-License-Identifier: MIT
 **
 **/

/********
 **
 **  socket.h: socket abstraction
 **
 **/
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cinttypes>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <netinet/in.h>
#include <optional>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <utility>
#include <vector>

#if !defined(LIBMU_SYSTEM_SOCKET_H_)
#define LIBMU_SYSTEM_SOCKET_H_

namespace libmu {
namespace system {

struct Socket {
  int sockfd_;

public:
#if 0
  int sockfd = socket(domain, type, protocol)
  int setsockopt(int sockfd, int level, int optname,  
                   const void *optval, socklen_t optlen);
  int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  int listen(int sockfd, int backlog);
  int new_socket= accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
  int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
#endif

  std::optional<int> MakeSocket(int, int, int) { return std::nullopt; }
  std::optional<int> SetSockOpt(int, const void *, socklen_t) {
    return std::nullopt;
  }
  std::optional<int> Listen(int) { return std::nullopt; }
  std::optional<int> Bind(const struct sockaddr *, socklen_t) {
    return std::nullopt;
  }
  std::optional<int> Accept(struct sockaddr *, socklen_t *) {
    return std::nullopt;
  }
  std::optional<int> Connect(int, const struct sockaddr *, socklen_t) {
    return std::nullopt;
  }

}; /* class Socket */

} /* namespace system */
} /* namespace libmu */

#endif /* LIBMU_SYSTEM_SOCKET_H_ */
