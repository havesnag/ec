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
std::mutex Loop::_sMutex;
std::map<uint32_t, ec::Loop *> Loop::_sLoops;
uint32_t Loop::_sIdGenerater = 0;

Loop::Loop() :
	_id(0),
	_base(NULL),
	_thread(NULL),
	_isStopping(false)
{
	_sMutex.lock();
	_sIdGenerater++;
	_id = _sIdGenerater;

	if (_sLoops.empty())
	{
#if (defined _WIN32) || (defined WIN32) || (defined _WIN64) || (defined WIN64)
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);
		evthread_use_windows_threads();
#else
		evthread_use_pthreads();
#endif//_WIN32
	}

	_sLoops[_id] = this;
	_sMutex.unlock();
	_base = event_base_new();
}

Loop::~Loop()
{
	if (NULL != _thread)
	{
		stop();
		wait();
		delete _thread;
	}

	if (NULL != _base)
	{
		event_base_free(_base);
	}

	_sMutex.lock();
	_sLoops.erase(_id);
	if (_sLoops.empty())
	{
#if (defined _WIN32) || (defined WIN32) || (defined _WIN64) || (defined WIN64)
		WSACleanup();
#endif//_WIN32
	}
	_sMutex.unlock();
}

bool Loop::start(bool newThread)
{
	if (!onBeforeStart()) {
		return false;
	}

	if (newThread)
	{
		_thread = new std::thread(std::bind(&Loop::run, this));
		return (NULL != _thread);
	}
	else
	{
		run();
		return true;
	}
}

void Loop::wait()
{
	if (NULL != _thread)
	{
		_thread->join();
	}
}

void Loop::stop(bool waiting)
{
	waiting ? event_base_loopexit(_base, NULL) : event_base_loopbreak(_base);
	onAfterStop();
}

void Loop::run()
{
	curThreadLoop = this;
	onBeforeLoop();
	event_base_loop(_base, 0);
	onAfterLoop();
	curThreadLoop = NULL;
	_thread = NULL;
}

bool Loop::onBeforeStart()
{
	return true;
}

void Loop::onBeforeLoop()
{
}

void Loop::onAfterLoop()
{
}

void Loop::onAfterStop()
{
}

Loop * Loop::curLoop()
{
	return curThreadLoop;
}

Loop * Loop::get(uint32_t id)
{
	std::unique_lock<std::mutex> lock(_sMutex);
	auto iter = _sLoops.find(id);
	return (iter != _sLoops.end()) ? iter->second : NULL;
}

} /* namespace ec */
