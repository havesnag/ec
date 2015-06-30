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
	_frameEvent(NULL),
	_frameRound(0),
	_isStopping(false),
	_asyncPendingCount(0)
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

void Loop::stop(bool pending)
{
	if (pending) {
		_isStopping = true;
	} else {
		event_base_loopexit(_base, NULL);
	}
}

void Loop::async(ec::Command cmd, uint8_t *data, uint16_t dataSize, ec::AsyncHandler handler)
{
	ec::AsyncContext *context = new ec::AsyncContext();

	context->cmd = cmd;
	context->request.set(data, dataSize);
	context->handler = handler;

	ec::Loop *curLoop = ec::Loop::curLoop();
	if (curLoop)
	{
		context->loopId = curLoop->getId();
		if (handler)
		{
			_asyncPendingCount++;
		}
	} else {
		context->loopId = 0;
	}

	_asyncRequests.push(context);
}

void Loop::post(uint8_t *data, uint16_t dataSize)
{
	_asyncPosts.push(new ec::Data(data, dataSize));
}

void Loop::regAsyncHandler(ec::Command cmd, ec::AsyncHandler handler)
{
	_asyncHandlers[cmd] = handler;
}

void Loop::regPostHandler(ec::PostHandler handler)
{
	_postHandler = handler;
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
	doEvent(ec::Loop::kEventRun);
	event_base_loop(_base, 0);
	doEvent(ec::Loop::kEventEnd);
	curThreadLoop = NULL;
	_thread = NULL;
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
	for (auto data : posts)
	{
		try
		{
			_postHandler ? _postHandler(*data) : onPost(*data);
		}
		catch (...) {}
		delete data;
	}

	//处理异步Call请求
	auto requests = _asyncRequests.moveAll();
	for (auto context : requests)
	{
		try
		{
			auto asyncHandlerIter = _asyncHandlers.find(context->cmd);
			(asyncHandlerIter != _asyncHandlers.end()) ?
				asyncHandlerIter->second(context->cmd, context->request, context->response) :
				onAsync(context->cmd, context->request, context->response);
		}
		catch (...) {};

		ec::Loop *fromLoop = ec::Loop::get(context->loopId);
		if ((NULL == fromLoop) || (!context->handler))
		{
			delete context;
			continue;
		}
		fromLoop->_asyncResponses.push(context);
	}

	//处理异步Call返回
	auto responses = _asyncResponses.moveAll();
	for (auto context : responses)
	{
		_asyncPendingCount--;
		context->handler(context->cmd, context->request, context->response);
		delete context;
	}

	//处理每幀逻辑
	try
	{
		_frameHandler ? _frameHandler(_frameRound) : onFrame(_frameRound);
	}
	catch (...) {}

	if (_isStopping && (_asyncPendingCount == 0))
	{
		stop(false);
	}
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
