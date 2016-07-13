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
	TcpSession();

	/** 获取管理此连接的调度器 */
	inline ec::TcpServerDispatcher * dispatcher() const
	{
		return _dispatcher;
	}

	/** 获取唯一会话id */
	inline ec::SessionId id() const
	{
		return _id;
	}

	/**
	 * @brief 关联调度器和套接字
	 * @return 是否成功
	 */
	bool attach(
			ec::TcpServerDispatcher * dispatcher,
			ec::SessionId id,
			ec::SocketFd sock);

protected:
	/**
	 * @brief 有数据可读时处理接口
	 * @details 默认行为就数据可读时转调TcpServer::onSessionRead，子类重载时如果没有调用
	 * 父类(TcpSession)的onRead，则TcpServer::onSessionRead也不会被调用，建议由Session
	 * 自己处理可读数据而通知到TcpServer处理
	 * @see TcpServer::onSessionRead
	 */
	virtual void onRead();

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
