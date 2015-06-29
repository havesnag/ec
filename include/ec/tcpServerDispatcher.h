/*
 * tcpServerDispatcher.h
 *
 *  Created on: 2015年6月25日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_TCPSERVERDISPATCHER_H_
#define INCLUDE_EC_TCPSERVERDISPATCHER_H_

#include "ec/loop.h"
#include "ec/tcpSession.h"

namespace ec
{

class TcpServer;
class TcpServerDispatcher : public ec::Loop
{
	friend class TcpSession;

	typedef std::recursive_mutex Mutex;
	typedef std::lock_guard<Mutex> MutexLock;
public:
	struct NewSessionData
	{
		ec::SocketFd sock;
		ec::SessionId id;
	};

public:
	TcpServerDispatcher(TcpServer *server);

	inline ec::TcpServer * getServer() const
	{
		return _server;
	}

	ec::TcpSessionPtr getSession(ec::SessionId id);

protected:
	/*
	 * @override
	 */
	virtual void onPost(ec::Data &data);

private:
	void addSession(ec::TcpSessionPtr session);
	void removeSession(ec::SessionId id);


private:
	TcpServer * _server;
	std::map<ec::SessionId, ec::TcpSessionPtr> _sessions;
	Mutex _mutex;
};

} /* namespace ec */



#endif /* INCLUDE_EC_TCPSERVERDISPATCHER_H_ */
