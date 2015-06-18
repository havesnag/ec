/*
 * loop.cpp
 *
 *  Created on: 2015年5月30日
 *      Author: havesnag
 */

#include "loop.h"

thread_local ec::Loop * curThreadLoop;

namespace ec
{
std::mutex Loop::s_mutex;
std::map<uint32_t, ec::Loop *> Loop::s_loops;

Loop::Loop(uint32_t id) :
	_id(id),
	_base(NULL),
	_thread(NULL)
{
	s_mutex.lock();
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

	s_mutex.lock();
	s_loops.erase(_id);
	if (s_loops.empty())
	{
		Loop::uninit();
	}
	s_mutex.unlock();
}

void Loop::setStartHandler(ec::EventHandler handler)
{
	_onStart = handler;
}

void Loop::setEndHandler(ec::EventHandler handler)
{
	_onEnd = handler;
}

bool Loop::run()
{
	curThreadLoop = this;
	return 0 == event_base_loop(_base, 0);
	curThreadLoop = NULL;
}

bool Loop::runThread()
{
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

Loop & Loop::getDefault()
{
	static Loop loop(0);
	return loop;
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
