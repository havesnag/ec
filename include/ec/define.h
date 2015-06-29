/*
 * define.h
 *
 *  Created on: 2015年5月30日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_DEFINE_H_
#define INCLUDE_EC_DEFINE_H_

#include <stdint.h>
#include <functional>
#include <algorithm>
#include <memory>

#include <event2/event.h>
#include <event2/thread.h>

namespace ec
{

typedef uint64_t SessionId;
const SessionId SESSION_ID_INVALID = 0;

typedef evutil_socket_t SocketFd;
const SocketFd SOCKET_FD_INVALID = -1;

typedef std::function<void ()> Handler;

} /* namespace ec */

#endif /* INCLUDE_EC_DEFINE_H_ */
