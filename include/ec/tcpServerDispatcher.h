/*
 * tcpServerDispatcher.h
 *
 *  Created on: 2015年6月25日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_TCPSERVERDISPATCHER_H_
#define INCLUDE_EC_TCPSERVERDISPATCHER_H_

#include <queue>
#include "ec/frameLoop.h"
#include "ec/tcpSession.h"
#include "ec/tcpSessionFactory.h"

namespace ec
{

class TcpServer;

/**
 * @brief TCP服务器会话调度管理器
 * @details TcpServer负责监听连接，然后分配给TcpServerDispatcher管理
 * @see ec::TcpServer
 */
class TcpServerDispatcher : public ec::FrameLoop
{
	typedef FrameLoop Super;

	struct SessionAction
	{
		ec::SessionId id;
		ec::SocketFd sock;
	};
public:
	TcpServerDispatcher(TcpServer *server);

	/** @brief 返回所在TCPServer */
	inline ec::TcpServer * server() const
	{
		return _server;
	}

	/** 根据id获取Session */
	ec::TcpSessionPtr getSession(ec::SessionId id);
	/** 添加Session */
	void addSession(ec::SessionId id, ec::SocketFd sock);
	/** 删除Session */
	void removeSession(ec::SessionId id);

protected:
	/** @override */
	virtual void onFrame();

private:
	TcpServer * _server;
	std::map<ec::SessionId, ec::TcpSessionPtr> _sessions;
	std::queue<SessionAction> _actions;
	Mutex _mutex;
};

} /* namespace ec */



#endif /* INCLUDE_EC_TCPSERVERDISPATCHER_H_ */
