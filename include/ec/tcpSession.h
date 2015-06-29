/*
 * tcpSession.h
 *
 *  Created on: 2015年6月22日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_TCPSESSION_H_
#define INCLUDE_EC_TCPSESSION_H_

#include "ec/tcpSocket.h"

namespace ec
{

class TcpServerDispatcher;
class TcpSession : public ec::TcpSocket, public std::enable_shared_from_this<TcpSession>
{
public:
	TcpSession(ec::TcpServerDispatcher *dispatcher, ec::SessionId id);

	inline ec::TcpServerDispatcher * getDispatcher() const
	{
		return _dispatcher;
	}

	inline ec::SessionId getId() const
	{
		return _id;
	}

	void attach(ec::SocketFd sock);

private:
	void handleEvent(short events);

	static void readCallback(struct bufferevent *bev, void *data);
	static void writeCallback(struct bufferevent *bev, void *data);
	static void eventCallback(struct bufferevent *bev, short events, void *data);

private:
	ec::TcpServerDispatcher *_dispatcher;
	ec::SessionId _id;
};

typedef std::shared_ptr<TcpSession> TcpSessionPtr;

} /* namespace ec */


#endif /* INCLUDE_EC_TCPSESSION_H_ */
