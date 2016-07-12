/*
 * tcpSession.cpp
 *
 *  Created on: 2015年6月22日
 *      Author: havesnag
 */

#include "ec/tcpSession.h"
#include "ec/tcpServerDispatcher.h"
#include "ec/tcpServer.h"

namespace ec
{

TcpSession::TcpSession(ec::TcpServerDispatcher *dispatcher, ec::SessionId id):
		_dispatcher(dispatcher),
		_id(id)
{
}

void TcpSession::attach(ec::SocketFd sock)
{
	if (SOCKET_FD_INVALID == sock)
	{
		return;
	}

	_bev = bufferevent_socket_new(_dispatcher->ev(),
			sock, BEV_OPT_THREADSAFE | BEV_OPT_DEFER_CALLBACKS);
	if (NULL == _bev)
	{
		return;
	}

	bufferevent_setcb(_bev, readCallback, writeCallback, eventCallback, this);
	if (-1 == bufferevent_enable(_bev, EV_READ | EV_WRITE))
	{
		bufferevent_free(_bev);
		_bev = NULL;
		return;
	}
}

void TcpSession::handleEvent(short events)
{
	bufferevent_disable(_bev, EV_READ | EV_WRITE);
	evutil_closesocket(getSocket());
	_dispatcher->server()->onSessionDisconnected(this);
	_dispatcher->removeSession(getId());
}

void TcpSession::readCallback(struct bufferevent *bev, void *data)
{
	TcpSession *session = (TcpSession *)data;
	session->_dispatcher->server()->onSessionRead(session);
}

void TcpSession::writeCallback(struct bufferevent *bev, void *data)
{
	TcpSession *session = (TcpSession *)data;
	session->checkClosing();
}

void TcpSession::eventCallback(struct bufferevent *bev, short events, void *data)
{
	TcpSession *session = (TcpSession *)data;
	session->handleEvent(events);
}

} /* namespace ec */
