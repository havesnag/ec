/*
 * async.h
 *
 *  Created on: 2015年6月22日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_ASYNC_H_
#define INCLUDE_EC_ASYNC_H_

#include <string.h>
#include "ec/define.h"
#include "ec/data.h"

namespace ec
{

typedef uint16_t Command;
typedef std::function<void (ec::Command, const ec::Data &, ec::Data &)> AsyncHandler;
typedef std::function<void (ec::Data &)> PostHandler;

struct AsyncContext
{
	uint32_t loopId;
	ec::Command cmd;
	ec::Data request;
	ec::Data response;
	ec::AsyncHandler handler;
};

} /* namespace ec */



#endif /* INCLUDE_EC_ASYNC_H_ */
