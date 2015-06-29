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

void TcpServerDispatcher::onPost(ec::Data &data)
{
	ec::TcpServerDispatcher::NewSessionData sessionData = data.get<ec::TcpServerDispatcher::NewSessionData>();

	TcpSessionPtr session(new TcpSession(this, sessionData.id));
	session->attach(sessionData.sock);
	if (!session->isInited())
	{
		return;
	}

	addSession(session);
	getServer()->onNewSession(session.get());
}

void TcpServerDispatcher::addSession(ec::TcpSessionPtr session)
{
	MutexLock lock(_mutex);
	_sessions[session->getId()] = session;
}

void TcpServerDispatcher::removeSession(ec::SessionId id)
{
	MutexLock lock(_mutex);
	_sessions.erase(id);
}

} /* namespace ec */
