/*
 * timer.h
 *
 *  Created on: 2015年5月31日
 *      Author: havesnag
 */

#ifndef EC_TIMER_H_
#define EC_TIMER_H_

#include "loop.h"

namespace ec
{

class Loop;
class Timer
{
public:
	typedef std::function<void ()> Handler;

public:
	Timer(const ec::Loop &loop);
	virtual ~Timer();

	/*
	 * @desc	:启动多轮循环定时器
	 * @param	:uint32_t interval: 定时时长
	 * 			 uint64_t round: 总周期
	 * 			 EventHandler handler: 定时回调函数
	 */
	bool startRounds(uint32_t interval, uint64_t round, ec::Timer::Handler handler);

	/*
	 * @desc	:启动单次定时器
	 * @param	:uint32_t interval: 定时时长
	 * 			 EventHandler handler: 定时回调函数
	 */
	bool startOnce(uint32_t interval, ec::Timer::Handler handler);

	/*
	 * @desc	:启动无限循环定时器
	 * @param	:uint32_t interval: 定时时长
	 * 			 EventHandler handler: 定时回调函数
	 */
	bool startForever(uint32_t interval, ec::Timer::Handler handler);

	/*
	 * @desc	:启动定时器
	 * @param	:uint32_t after: 在此时长后开始定时，以毫秒为单位
	 * 			 uint32_t interval: 定时时长，以毫秒为单位
	 * 			 uint64_t round: 总周期
	 * 			 EventHandler handler: 定时回调函数
	 */
	bool startAfter(
			uint32_t after,
			uint32_t interval,
			uint64_t round,
			ec::Timer::Handler handler);


	inline uint32_t getInterval() const
	{
		return _interval;
	}

	inline uint64_t getRound() const
	{
		return _round;
	}

	inline uint64_t getCurRound() const
	{
		return _curRound;
	}

	inline bool isFinished() const
	{
		return _curRound >= _round;
	}

private:
	void reset();
	static void eventHandler(evutil_socket_t fd, short events, void *ctx);

private:
	const Loop &_loop;
	struct event *_event;
	uint32_t _interval; //定时时长，以毫秒为单位
	uint64_t _round; //总周期
	uint64_t _curRound; //当前周期
	ec::Timer::Handler _handler;
};

} /* namespace ec */

#endif /* EC_TIMER_H_ */
