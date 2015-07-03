/*
 * loop.h
 *
 *  Created on: 2015年5月30日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_LOOP_H_
#define INCLUDE_EC_LOOP_H_

#include <thread>
#include <mutex>
#include <atomic>
#include <map>

#include "ec/define.h"
#include "ec/safeQueue.h"
#include "ec/async.h"

namespace ec
{

/**
 * @brief 事件主循环类，一个线程只能有一个Loop对象。
 * @details 封装了libevent的struct event_base时间循环机制；
 * @details 内部以5毫秒每幀的频率处理异步请求
 */
class Loop
{
public:
	/** @brief Loop内部事件枚举 */
	enum Event
	{
		kEventStart = 0,/** 启动前将触发此事件 */
		kEventRun, 		/** 事件循环开始前将触发此事件 */
		kEventEnd, 		/** 事件循环结束后将触发此事件 */
		kEventMax
	};

	/**
	 * @brief 事件处理函数
	 * @param event Loop内部事件
	 * @return 返回错误码，0表示正确
	 */
	typedef std::function<bool (ec::Loop::Event event)> EventHandler;

	/**
	 * @brief 每幀处理函数
	 * @param frame 当前幀号
	 */
	typedef std::function<void (uint64_t frame)> FrameHandler;

public:
	Loop();
	virtual ~Loop();

	/** @brief 转换为event_base *指针 */
	inline operator event_base * () const
	{
		return _base;
	};

	/** @brief 获取event_base *指针*/
	inline event_base * getBase() const
	{
		return _base;
	}

	/** @brief 获得自动生成的Id号 */
	inline uint32_t getId() const
	{
		return _id;
	}

	/** @brief 获得经过的幀数 */
	inline uint64_t getFrameRound() const
	{
		return _frameRound;
	}

	/** @brief 是否是以新线程在运行 */
	inline bool isThread() const
	{
		return (NULL != _thread);
	}

	/**
	 * @brief 在当前线程运行事件循环
	 * @details 运行成功后会阻塞知道停止
	 * @return 是否成功
	 */
	bool start();


	/**
	 * @brief 启动新线程运行事件循环
	 * @return 是否成功
	 */
	bool startThread();

	/**
	 * @brief 等待线程结束，与startThread配合使用
	 * @see bool Loop::startThread()
	 */
	void waitThread();

	/**
	 * @brief 停止事件循环
	 * @details 如果以新线程运行的，则会停止新线程。
	 * @param pending 是否等待事件结束才停止
	 */
	void stop(bool pending = true);

	/**
	 * @brief 异步调用命令
	 * @param cmd 命令码
	 * @param data 数据块地址
	 * @param dataSize 数据块大小
	 * @param handler 命令回调函数
	 * @see ec::AsyncHandler
	 */
	void async(ec::Command cmd, uint8_t *data, uint16_t dataSize, ec::AsyncHandler handler);

	/**
	 * @brief 异步调用命令
	 * @param cmd 命令码
	 * @param value 数据
	 * @param handler 命令回调函数
	 * @see AsyncHandler
	 */
	template <typename T>
	void async(ec::Command cmd, T &value, ec::AsyncHandler handler)
	{
		async(cmd, (uint8_t *)(&value), sizeof(value), handler);
	}

	/**
	 * @brief 异步发送数据
	 * @param value 数据
	 */
	void post(uint8_t *data, uint16_t dataSize);

	/**
	 * @brief 异步发送数据
	 * @param data 数据块地址
	 * @param dataSize 数据块大小
	 */
	template <typename T>
	void post(T value)
	{
		post((uint8_t *)(&value), sizeof(value));
	}

	/** @brief 注册async回调，没有注册回调的异步调用将通过onAsync处理 */
	void regAsyncHandler(ec::Command cmd, ec::AsyncHandler handler);
	/** @brief 注册post回调，不注册将通过onPost处理 */
	void regPostHandler(ec::PostHandler handler);
	/** @brief 注册内部事件回调，没有注册回调的事件将通过onEvent处理 */
	void regEventHandler(ec::Loop::Event event, ec::Loop::EventHandler handler);
	/** @brief 注册每幀回调，不注册将通过onFrame处理 */
	void regFrameHandler(ec::Loop::FrameHandler handler);

protected:
	/** @brief 继承此接口处理异步命令调用 */
	virtual void onAsync(ec::Command cmd, const ec::Data &request, ec::Data &response) {};
	/** @brief 继承此接口处理异步数据 */
	virtual void onPost(ec::Data &data) {};
	/** @brief 继承此接口处理内部事件 */
	virtual bool onEvent(ec::Loop::Event event) { return true; };
	/** @brief 继承此接口处理每幀逻辑 */
	virtual void onFrame(uint64_t round) {};

private:
	void run();

	static void frameEventCallback(evutil_socket_t fd, short events, void *ctx);
	void frameHandler();

	bool doEvent(ec::Loop::Event event);

private:
	uint32_t _id;
	event_base *_base;
	//线程
	std::thread *_thread;
	//幀定时句柄
	struct event *_frameEvent;
	//当前幀数
	uint64_t _frameRound;
	std::atomic<bool> _isStopping;

	//异步post列表
	ec::SafeQueue<ec::Data *> _asyncPosts;
	//异步命令请求列表
	ec::SafeQueue<ec::AsyncContext *> _asyncRequests;
	//异步命令返回列表
	ec::SafeQueue<ec::AsyncContext *> _asyncResponses;
	//未完成的异步请求数量
	std::atomic<int> _asyncPendingCount;

	std::map<ec::Command, ec::AsyncHandler> _asyncHandlers;
	ec::PostHandler _postHandler;
	ec::Loop::EventHandler _eventHandlers[ec::Loop::kEventMax];
	ec::Loop::FrameHandler _frameHandler;

public:
	/**
	 * @brief 返回当前线程的Loop
	 * @return 当前线程Loop指针，没有Loop则NULL
	 */
	static Loop * curLoop();

	/**
	 * @brief 根据id查找Loop
	 * @return Loop指针
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

#endif /* INCLUDE_EC_LOOP_H_ */
