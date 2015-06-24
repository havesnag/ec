/*
 * loop.cpp
 *
 *  Created on: 2015年5月30日
 *      Author: havesnag
 */

#include "ec/loop.h"

thread_local ec::Loop * curThreadLoop;

namespace ec
{
std::mutex Loop::s_mutex;
std::map<uint32_t, ec::Loop *> Loop::s_loops;
uint32_t Loop::s_idGenerater = 0;

Loop::Loop() :
	_id(0),
	_base(NULL),
	_thread(NULL),
	_frameEvent(NULL)
{
	s_mutex.lock();
	s_idGenerater++;
	_id = s_idGenerater;
	if (s_loops.empty())
	{
		Loop::init();
	}
	s_loops[_id] = this;
	s_mutex.unlock();

	_base = event_base_new();
}

Loop::~Loop()
{
	event_base_free(_base);
	if (NULL != _thread)
	{
		delete _thread;
	}

	if (NULL != _frameEvent)
	{
		event_free(_frameEvent);
		_frameEvent = NULL;
	}

	s_mutex.lock();
	s_loops.erase(_id);
	if (s_loops.empty())
	{
		Loop::uninit();
	}
	s_mutex.unlock();
}

bool Loop::start()
{
	if (!doEvent(ec::Loop::kEventStart))
	{
		return false;
	}

	run();
	return true;
}
bool Loop::startThread()
{
	if (!doEvent(ec::Loop::kEventStart))
	{
		return false;
	}

	_thread = new std::thread(std::bind(&Loop::run, this));
	return true;
}

void Loop::waitThread()
{
	if (NULL != _thread)
	{
		_thread->join();
	}
}

void Loop::stop()
{
	event_base_loopexit(_base, NULL);
}

void Loop::call(uint16_t cmd, uint8_t *data, uint16_t dataSize, ec::CommandHandler handler)
{
	ec::async::Call *call = new ec::async::Call();

	call->cmd = cmd;
	call->data.set(data, dataSize);
	call->handler = handler;

	ec::Loop *curLoop = ec::Loop::curLoop();
	call->loopId = (NULL != curLoop) ? curLoop->getId() : 0;

	_asyncCallRequests.push(call);
}

void Loop::post(uint16_t cmd, uint8_t *data, uint16_t dataSize)
{
	ec::async::Post *post = new ec::async::Post();
	post->cmd = cmd;
	post->data.set(data, dataSize);
	_asyncPosts.push(post);
}

void Loop::regCommandHandler(ec::Command cmd, ec::CommandHandler handler)
{
	_commandHandlers[cmd] = handler;
}

void Loop::regEventHandler(ec::Loop::Event event, ec::Loop::EventHandler handler)
{
	_eventHandlers[event] = handler;
}

void Loop::regFrameHandler(ec::Loop::FrameHandler handler)
{
	_frameHandler = handler;
}

void Loop::run()
{
	_frameEvent = event_new(_base, -1, EV_PERSIST, frameEventCallback, this);
	if (NULL == _frameEvent)
	{
		return;
	}

	struct timeval tv = {};
	evutil_timerclear(&tv);
	tv.tv_sec = 0;
	tv.tv_usec = 5000;

	if (0 != event_add(_frameEvent, &tv))
	{
		event_free(_frameEvent);
		_frameEvent = NULL;
		return;
	}

	curThreadLoop = this;
	event_base_loop(_base, 0);
	curThreadLoop = NULL;
}

void Loop::frameEventCallback(evutil_socket_t fd, short events, void *ctx)
{
	ec::Loop *loop = (ec::Loop *)ctx;
	loop->frameHandler();
}

void Loop::frameHandler()
{
	_frameRound++;

	//处理异步Post
	auto posts = _asyncPosts.moveAll();
	for (auto post : posts)
	{
		doCommand(post->cmd, post->data);
		delete post;
	}

	//处理异步Call请求
	auto requests = _asyncCallRequests.moveAll();
	for (auto call : requests)
	{
		doCommand(call->cmd, call->data);
		ec::Loop *fromLoop = ec::Loop::get(call->loopId);
		if ((NULL == fromLoop) || (!call->handler))
		{
			delete call;
			continue;
		}
		fromLoop->_asyncCallResponses.push(call);
	}

	//处理异步Call返回
	auto responses = _asyncCallResponses.moveAll();
	for (auto call : responses)
	{
		call->handler(call->cmd, call->data);
		delete call;
	}

	//处理每幀逻辑
	try
	{
		_frameHandler ? _frameHandler(_frameRound) : onFrame(_frameRound);
	}
	catch (...) {}
}

bool Loop::doEvent(ec::Loop::Event event)
{
	try
	{
		ec::Loop::EventHandler handler = _eventHandlers[event];
		return handler ? handler(event) : onEvent(event);
	}
	catch (...)
	{
		return false;
	}
}

void Loop::doCommand(ec::Command cmd, ec::Data &data)
{
	try
	{
		auto iter = _commandHandlers.find(cmd);
		(iter != _commandHandlers.end()) ? iter->second(cmd, data) : onCommand(cmd, data);
	}
	catch (...) {}
}

Loop * Loop::curLoop()
{
	return curThreadLoop;
}

Loop * Loop::get(uint32_t id)
{
	std::unique_lock<std::mutex> lock(s_mutex);
	auto iter = s_loops.find(id);
	return (iter != s_loops.end()) ? iter->second : NULL;
}

void Loop::init()
{
#if (defined _WIN32) || (defined WIN32) || (defined _WIN64) || (defined WIN64)
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif//_WIN32
}

void Loop::uninit()
{
#if (defined _WIN32) || (defined WIN32) || (defined _WIN64) || (defined WIN64)
	WSACleanup();
#endif//_WIN32
}

} /* namespace ec */
