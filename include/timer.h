/*
 * timer.h
 *
 *  Created on: 2015年5月31日
 *      Author: havesnag
 */

#ifndef EC_TIMER_H_
#define EC_TIMER_H_

#include "loop.h"
#include <atomic>

namespace ec
{

class Timer
{
public:
	Timer(const Loop &loop);
	virtual ~Timer();

	/*
	 * @desc	:启动多轮循环定时器
	 * @param	:uint32_t interval: 定时时长
	 * 			 uint32_t round: 总周期
	 * 			 EventHandler handler: 定时回调函数
	 */
	bool startRounds(uint32_t interval, uint32_t round, ec::EventHandler handler);

	/*
	 * @desc	:启动单次定时器
	 * @param	:uint32_t interval: 定时时长
	 * 			 EventHandler handler: 定时回调函数
	 */
	bool startOnce(uint32_t interval, ec::EventHandler handler);

	/*
	 * @desc	:启动无限循环定时器
	 * @param	:uint32_t interval: 定时时长
	 * 			 EventHandler handler: 定时回调函数
	 */
	bool startForever(uint32_t interval, ec::EventHandler handler);

	/*
	 * @desc	:启动定时器
	 * @param	:uint32_t after: 在此时长后开始定时，以毫秒为单位
	 * 			 uint32_t interval: 定时时长，以毫秒为单位
	 * 			 uint32_t round: 总周期
	 * 			 EventHandler handler: 定时回调函数
	 */
	bool startAfter(
			uint32_t after,
			uint32_t interval,
			uint32_t round,
			ec::EventHandler handler);


	inline uint32_t getInterval() const
	{
		return _interval;
	}

	inline uint32_t getRound() const
	{
		return _round;
	}

	inline uint32_t getCurRound() const
	{
		return _curRound;
	}

private:
	void reset();

private:
	static void eventHandler(evutil_socket_t fd, short events, void *ctx);

private:
	const Loop &_loop;
	struct event *_event;
	uint32_t _interval; //定时时长，以毫秒为单位
	uint32_t _round; //总周期
	std::atomic<uint32_t> _curRound; //当前周期
	ec::EventHandler _handler;
};

} /* namespace ec */

#endif /* EC_TIMER_H_ */
