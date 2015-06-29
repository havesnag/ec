/*
 * tcpClient.h
 *
 *  Created on: 2015年6月25日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_TCPCLIENT_H_
#define INCLUDE_EC_TCPCLIENT_H_

#include "ec/tcpSocket.h"
#include "ec/loop.h"

namespace ec
{

class TcpClient : public TcpSocket
{
public:
	TcpClient(const ec::Loop &loop);

	inline bool isConnected() const
	{
		return _isConnected;
	}

	bool connect(const char *ip, uint16_t port);

protected:
	virtual void onRead() {};
	virtual void onDisconnected() {};
	virtual void onConnected(int error) {};

private:
	void handleEvent(short events);

	static void readCallback(struct bufferevent *bev, void *ctx);
	static void writeCallback(struct bufferevent *bev, void *ctx);
	static void eventCallback(struct bufferevent *bev, short events, void *ctx);

private:
	const Loop &_loop;
	std::atomic<bool> _isConnected;
};

} /* namespace ec */


#endif /* INCLUDE_EC_TCPCLIENT_H_ */
