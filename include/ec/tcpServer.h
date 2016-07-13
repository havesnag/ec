/*
 * tcpServer.h
 *
 *  Created on: 2015年6月22日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_TCPSERVER_H_
#define INCLUDE_EC_TCPSERVER_H_

#include <string>
#include <map>
#include <vector>
#include <event2/listener.h>
#include "ec/define.h"
#include "ec/tcpServerDispatcher.h"

namespace ec
{

/**
 * @brief TCP服务器
 * @details 其包含了一个监听线程和多个IO线程
 */
class TcpServer
{
	friend class TcpSession;
	friend class TcpServerDispatcher;
public:
	TcpServer(ec::TcpSessionFactory * sessionFactory = NULL, uint16_t threads = 0);
	virtual ~TcpServer();

	/** 设置IO线程数量，需在监听之前调用 */
	void setThreads(uint16_t threads);

	/** 获取Loop对象引用 */
	inline ec::Loop & getMasterLoop() const
	{
		return *_master;
	}

	/** 获取Session工厂类 */
	inline ec::TcpSessionFactory * sessionFactory() const
	{
		return _sessionFactory;
	}

	/**
	 * @brief 监听
	 * @param ip 服务器IP地址
	 * @param port 服务器监听端口
	 * @return 返回是否成功
	 */
	bool listen(const char *ip, uint16_t port);

	/** 停止 */
	void stop();

	/** 等待所有线程结束 */
	void wait();

protected:
	/**
	 * @brief 监听失败时处理接口
	 * @details 比如监听端口被占用时
	 */
	virtual void onListenError() {};

	/**
	 * @brief 连接有数据可读时处理接口
	 * @note TcpSession数据可读时默认行为是通知TcpServer，重载TcpSession的onRead函数修改
	 * @see TcpSession::onRead
	 */
	virtual void onSessionRead(ec::TcpSession *session) {};

	/** 连接断开时处理接口 */
	virtual void onSessionDisconnected(ec::TcpSession *session) {};

	/** 有新连接时处理接口 */
	virtual void onNewSession(ec::TcpSession *session) {};

private:
	ec::TcpServerDispatcher * getSessionDispatcher(ec::SessionId id);

	static void listenEventCallback(
		struct evconnlistener *listener,
		evutil_socket_t sock,
		struct sockaddr *addr,
		int len,
		void *ctx);

	static void listenErrorCallback(struct evconnlistener *listener, void *ctx);

private:
	struct evconnlistener *_eventListener;
	ec::TcpServerDispatcher * _master;
	std::vector<ec::TcpServerDispatcher *> _slavers;
	ec::TcpSessionFactory * _sessionFactory;

private:
	static ec::SessionId s_sessionIdGenerator;

};

} /* namespace ec */

#endif /* INCLUDE_EC_TCPSERVER_H_ */
