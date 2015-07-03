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

/**
 * @brief 服务端TCP连接
 */
class TcpSession : public ec::TcpSocket, public std::enable_shared_from_this<TcpSession>
{
public:
	TcpSession(ec::TcpServerDispatcher *dispatcher, ec::SessionId id);

	/** 获取管理此连接的调度器 */
	inline ec::TcpServerDispatcher * getDispatcher() const
	{
		return _dispatcher;
	}

	/** 获取唯一会话id */
	inline ec::SessionId getId() const
	{
		return _id;
	}

	/** 关联到某socket */
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
