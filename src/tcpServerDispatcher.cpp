/*
 * tcpServerDispatcher.cpp
 *
 *  Created on: 2015年6月25日
 *      Author: havesnag
 */

#include "ec/tcpServerDispatcher.h"
#include "ec/tcpServer.h"

namespace ec
{

TcpServerDispatcher::TcpServerDispatcher(TcpServer *server) :
		_server(server)
{}

ec::TcpSessionPtr TcpServerDispatcher::getSession(ec::SessionId id)
{
	MutexLock lock(_mutex);
	auto iter = _sessions.find(id);
	return (iter != _sessions.end()) ? iter->second : NULL;
}

void TcpServerDispatcher::addSession(ec::SessionId id, ec::SocketFd sock)
{
	MutexLock lock(_mutex);
	_actions.push({id, sock});
}

void TcpServerDispatcher::removeSession(ec::SessionId id)
{
	MutexLock lock(_mutex);
	_actions.push({id, SOCKET_FD_INVALID});
}

void TcpServerDispatcher::onFrame()
{
	if (_actions.empty())
	{
		return;
	}

	_mutex.lock();
	while (!_actions.empty())
	{
		SessionAction & action = _actions.front();

		if (action.sock != SOCKET_FD_INVALID) //新加session
		{
			TcpSessionPtr session(new TcpSession(this, action.id));
			session->attach(action.sock);
			if (session->isInited())
			{
				_sessions[session->getId()] = session;
			}
		}
		else //删除session
		{
			_sessions.erase(action.id);
		}

		_actions.pop();
	}
}


} /* namespace ec */
