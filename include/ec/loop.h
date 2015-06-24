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
	enum Event
	{
		kEventStart = 0,	/* 启动前将触发此事件 */
		kEventRun, 		/* 事件循环开始前将触发此事件 */
		kEventEnd, 		/* 事件循环结束后将触发此事件 */
		kEventCount
	};

	/* 事件处理函数，返回错误码，0表示成功 */
	typedef std::function<bool (ec::Loop::Event)> EventHandler;
	/* 每幀处理函数 */
	typedef std::function<void (uint64_t)> FrameHandler;

public:
	Loop();
	virtual ~Loop();

	inline operator event_base * () const
	{
		return _base;
	};

	inline uint32_t getId() const
	{
		return _id;
	}

	inline uint64_t getFrameRound() const
	{
		return _frameRound;
	}

	/*
	 * @desc	:在当前线程运行事件循环
	 * @return	:是否成功
	 */
	bool start();


	/*
	 * @desc	:启动新线程运行事件循环
	 * @return	:是否成功
	 */
	bool startThread();

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

	//
	//注册命令回调，没有注册回调的命令将通过onCommand处理
	void regCommandHandler(ec::Command cmd, ec::CommandHandler handler);
	//注册事件回调，没有注册回调的事件将通过onEvent处理
	void regEventHandler(ec::Loop::Event event, ec::Loop::EventHandler handler);
	//注册每幀回调，没有注册回调将通过onFrame处理
	void regFrameHandler(ec::Loop::FrameHandler handler);

protected:
	virtual bool onEvent(ec::Loop::Event event) { return true; };
	virtual void onCommand(ec::Command cmd, ec::Data &data) {};
	virtual void onFrame(uint64_t round) {};

private:
	void run();

	static void frameEventCallback(evutil_socket_t fd, short events, void *ctx);
	void frameHandler();

	bool doEvent(ec::Loop::Event event);
	void doCommand(ec::Command cmd, ec::Data &data);

private:
	uint32_t _id;
	event_base *_base;
	std::thread *_thread;
	struct event *_frameEvent;
	uint64_t _frameRound;

	ec::SafeQueue<ec::async::Post *> _asyncPosts;
	ec::SafeQueue<ec::async::Call *> _asyncCallRequests;
	ec::SafeQueue<ec::async::Call *> _asyncCallResponses;

	std::map<ec::Command, ec::CommandHandler> _commandHandlers;
	ec::Loop::EventHandler _eventHandlers[ec::Loop::kEventCount];
	ec::Loop::FrameHandler _frameHandler;

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
