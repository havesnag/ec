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

/**
 * @brief 命令码
 */
typedef uint16_t Command;

/**
 * @brief 异步命令请求处理函数
 * @param cmd 请求命令码
 * @param request 请求数据，只读
 * @param response 返回数据，可写，修改它向发起异步调用的Loop返回数据
 */
typedef std::function<void (ec::Command cmd, const ec::Data &request, ec::Data &response)> AsyncHandler;

/**
 * @brief 异步数据处理函数
 * @param data 请求数据
 */
typedef std::function<void (ec::Data &data)> PostHandler;

/** @brief 异步命令请求内容 */
struct AsyncContext
{
	uint32_t loopId; /** 发起异步命令请求的Loop标号 */
	ec::Command cmd; /** 请求命令码 */
	ec::Data request; /** 请求数据 */
	ec::Data response; /** 返回数据 */
	ec::AsyncHandler handler; /** 回调函数 */
};

} /* namespace ec */



#endif /* INCLUDE_EC_ASYNC_H_ */
