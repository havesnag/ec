/*
 * command.h
 *
 *  Created on: 2015年6月22日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_COMMAND_H_
#define INCLUDE_EC_COMMAND_H_

#include <string.h>
#include "ec/define.h"
#include "ec/data.h"

namespace ec
{

typedef uint16_t Command;
typedef std::function<void (ec::Command cmd, ec::Data &data)> CommandHandler;

namespace async
{

struct Post
{
	ec::Command cmd;
	ec::Data data;
};

struct Call
{
	uint32_t loopId;
	ec::Command cmd;
	ec::Data data;
	ec::CommandHandler handler;
};

} /* namespace async */

} /* namespace ec */



#endif /* INCLUDE_EC_COMMAND_H_ */
