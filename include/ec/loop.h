/*
 * loop.h
 *
 *  Created on: 2015年5月30日
 *      Author: havesnag
 */

#ifndef EC_LOOP_H_
#define EC_LOOP_H_

#include <thread>
#include <mutex>
#include <map>

#include <event2/event.h>
#include <event2/thread.h>

#include "ec/define.h"
#include "ec/safeQueue.h"
#include "ec/command.h"

namespace ec
{

class Loop
{
public:
	Loop();
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
	void run();

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

	/*
	 * @desc	:异步调用命令
	 */
	void call(uint16_t cmd, uint8_t *data, uint16_t dataSize, ec::CommandHandler handler);

	/*
	 * @desc	:异步发送数据
	 */
	void post(uint16_t cmd, uint8_t *data, uint16_t dataSize);

	void regCommandHandler(ec::Command cmd, ec::CommandHandler handler);

private:
	static void frameEventCallback(evutil_socket_t fd, short events, void *ctx);
	void frameHandler();

private:
	uint32_t _id;
	event_base *_base;
	std::thread *_thread;
	struct event *_frameEvent;

	ec::SafeQueue<ec::async::Post *> _asyncPosts;
	ec::SafeQueue<ec::async::Call *> _asyncCallRequests;
	ec::SafeQueue<ec::async::Call *> _asyncCallResponses;

	std::map<ec::Command, ec::CommandHandler> _commandHandlers;

	EventHandler _onStart;
	EventHandler _onEnd;


public:
	/*
	 * @desc	:返回当前线程的Loop
	 */
	static Loop * curLoop();

	/*
	 * @desc	:根据id查找Loop
	 */
	static Loop * get(uint32_t id);

private:
	static void init();
	static void uninit();

private:
	static std::mutex s_mutex;
	static std::map<uint32_t, ec::Loop *> s_loops;
	static uint32_t s_idGenerater;
};

} /* namespace ec */

#endif /* EC_LOOP_H_ */
