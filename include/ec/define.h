/*
 * define.h
 *
 *  Created on: 2015年5月30日
 *      Author: havesnag
 */

#ifndef EC_DEFINE_H_
#define EC_DEFINE_H_

#include <event2/event.h>
#include <event2/thread.h>
#include <functional>

namespace ec
{

typedef evutil_socket_t SocketFd;
const SocketFd SOCKET_FD_INVALID = -1;

typedef std::function<void ()> EventHandler;

}

#endif /* EC_DEFINE_H_ */
