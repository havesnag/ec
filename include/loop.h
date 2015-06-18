/*
 * loop.h
 *
 *  Created on: 2015年5月30日
 *      Author: havesnag
 */

#ifndef EC_LOOP_H_
#define EC_LOOP_H_

#include "define.h"
#include <thread>
#include <mutex>
#include <map>

namespace ec
{

class Loop
{
public:
	Loop(uint32_t id = 0);
	virtual ~Loop();

	inline operator event_base * () const
	{
		return _base;
	};

	inline uint32_t getId()
	{
		return _id;
	}

	/*
	 * @desc	:设计开始事件循环前的回调还数
	 */
	void setStartHandler(ec::EventHandler handler);

	/*
	 * @desc	:设计结束事件循环后的回调还数
	 */
	void setEndHandler(ec::EventHandler handler);

	/*
	 * @desc	:在当前线程运行事件循环
	 * @return	:师傅成功
	 */
	bool run();

	/*
	 * @desc	:启动新线程运行事件循环
	 * @return	:师傅成功
	 */
	bool runThread();

	/*
	 * @desc	:等待线程结束
	 */
	void waitThread();

	/*
	 * @desc	:停止事件循环
	 * @note	:如果以新线程运行的，则会停止新线程。
	 */
	void stop();

private:
	uint32_t _id;
	event_base *_base;
	std::thread *_thread;

	EventHandler _onStart;
	EventHandler _onEnd;


public:
	/*
	 * @desc	:获得默认的Loop
	 */
	static Loop & getDefault();

	/*
	 * @desc	:返回当前线程的Loop
	 */
	static Loop * curLoop();

	/*
	 * @desc	:返回当前线程的Loop
	 */
	static Loop * get(uint32_t id);

private:
	static void init();
	static void uninit();

private:
	static std::mutex s_mutex;
	static std::map<uint32_t, ec::Loop *> s_loops;
};

} /* namespace ec */

#endif /* EC_LOOP_H_ */
