/*
 * tcpServer.cpp
 *
 *  Created on: 2015年6月22日
 *      Author: havesnag
 */

#include "ec/tcpServer.h"

namespace ec
{

ec::SessionId TcpServer::s_sessionIdGenerator = 0;

TcpServer::TcpServer(uint16_t threads) :
		_eventListener(NULL),
		_master(NULL)
{
	_master = new ec::TcpServerDispatcher(this);
	setThreads(threads);
}

TcpServer::~TcpServer()
{
	delete _master;
	for (auto dispatcher : _slavers)
	{
		delete dispatcher;
	}

	if (NULL != _eventListener)
	{
		evconnlistener_free(_eventListener);
	}
}

void TcpServer::setThreads(uint16_t threads)
{
	if (!_slavers.empty())
	{
		return;
	}

	for (uint16_t i = 0; i < threads; ++i)
	{
		_slavers.push_back(new ec::TcpServerDispatcher(this));
	}
}

bool TcpServer::listen(const char *ip, uint16_t port)
{
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	evutil_inet_pton(AF_INET, ip, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(port);

	_eventListener = evconnlistener_new_bind(_master->getBase(),
			listenEventCallback, this,
			LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE, -1,
			(struct sockaddr *)&serverAddr, sizeof(serverAddr));

	if (NULL == _eventListener)
	{
		return false;
	}

	evutil_make_socket_nonblocking(evconnlistener_get_fd(_eventListener));
	evconnlistener_set_error_cb(_eventListener, listenErrorCallback);

	if (!_master->startThread())
	{
		evconnlistener_free(_eventListener);
		_eventListener = NULL;
		return false;
	}

	for (auto dispatcher : _slavers)
	{
		dispatcher->startThread();
	}

	return true;
}

void TcpServer::stop()
{
	_master->stop();
	for (auto dispatcher : _slavers)
	{
		dispatcher->stop();
	}
}

void TcpServer::wait()
{
	_master->waitThread();
	for (auto dispatcher : _slavers)
	{
		dispatcher->waitThread();
	}
}

ec::TcpServerDispatcher * TcpServer::getSessionDispatcher(ec::SessionId id)
{
	return _slavers.empty() ? _master : _slavers[id%_slavers.size()];
}

void TcpServer::listenEventCallback(
	struct evconnlistener *listener,
	evutil_socket_t sock,
	struct sockaddr *addr,
	int len,
	void *ctx)
{
	TcpServer *server = (TcpServer *)ctx;
	evutil_make_socket_nonblocking(sock);
	ec::SessionId id = ++s_sessionIdGenerator;
	ec::TcpServerDispatcher::NewSessionData data = {sock, id};
	ec::TcpServerDispatcher *dispatcher = server->getSessionDispatcher(id);
	dispatcher->post<ec::TcpServerDispatcher::NewSessionData>(data);
}

void TcpServer::listenErrorCallback(struct evconnlistener *listener, void *ctx)
{
	TcpServer *server = (TcpServer *)ctx;
	if (NULL != server)
	{
		server->onListenError();
	}
}


} /* namespace ec */


