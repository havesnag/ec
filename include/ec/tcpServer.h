/*
 * tcpServer.h
 *
 *  Created on: 2015年6月22日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_TCPSERVER_H_
#define INCLUDE_EC_TCPSERVER_H_

#include <map>
#include <vector>
#include <event2/listener.h>
#include "ec/define.h"
#include "ec/tcpServerDispatcher.h"

namespace ec
{

class TcpServer
{
	friend class TcpSession;
	friend class TcpServerDispatcher;
public:
	TcpServer(uint16_t threads = 0);
	virtual ~TcpServer();

	void setThreads(uint16_t threads);

	inline const ec::Loop & getLoop() const
	{
		return *_master;
	}

	bool listen(const char *ip, uint16_t port);
	bool start();
	bool startThread();
	void waitThrad();
	void stop();

protected:
	virtual void onListenError() {};
	virtual void onSessionRead(ec::TcpSession *session) {};
	virtual void onSessionDisconnected(ec::TcpSession *session) {};
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

private:
	static ec::SessionId s_sessionIdGenerator;

};

} /* namespace ec */

#endif /* INCLUDE_EC_TCPSERVER_H_ */
