/*
 * tcpExample.cpp
 *
 *  Created on: 2015年6月29日
 *      Author: havesnag
 */

#include <iostream>
#include <functional>
using namespace std;

#include "ec/loop.h"
#include "ec/tcpServer.h"
#include "ec/tcpClient.h"
#include "ec/timer.h"
#include "ec/singleton.h"

#include "index.h"

class ExampleTcpClient : public ec::TcpClient
{
public:
	ExampleTcpClient(const ec::Loop &loop):
		ec::TcpClient(loop)
	{}

protected:
	virtual void onRead()
	{
	}

	virtual void onDisconnected()
	{
		cout << "ExampleTcpClient::" << __FUNCTION__ << endl;
	}

	virtual void onConnected(int error)
	{
		cout << "ExampleTcpClient::" << __FUNCTION__ << endl;
		send("hello", 5);
	}
};

class ExampleTcpServer : public ec::TcpServer
{
protected:
	virtual void onListenError()
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
	}

	virtual void onSessionRead(ec::TcpSession *session)
	{
		uint32_t length = session->getInputBufferLength();
		char * buff = new char[length + 1];
		session->readInputBuffer((uint8_t *)buff, length);
		buff[length] = '\0';
		cout << "ExampleTcpServer::" << __FUNCTION__
				<< " " << session->getId() << " data:" << buff << endl;
		session->close();
	}

	virtual void onSessionDisconnected(ec::TcpSession *session)
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
	}

	virtual void onNewSession(ec::TcpSession *session)
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << session->getId() << endl;
	}
};

typedef ec::Singleton<ExampleTcpServer> ExampleTcpServerSingleton;

class ExampleTcpClientManager : public ec::Loop
{
public:
	ExampleTcpClientManager():
		_client(*this),
		_timer(*this)
	{
		_timer.startForever(1000, std::bind(&ExampleTcpClientManager::tick, this));
	}

protected:
	void tick()
	{
		cout << "round " << _timer.getCurRound() << endl;
		if (!_client.isConnected())
		{
			_client.connect("127.0.0.1", 4567);
		}

		if (_timer.getCurRound() >= 3)
		{
			ExampleTcpServerSingleton::instance().stop();
			stop();
		}
	}
private:
	ExampleTcpClient _client;
	ec::Timer _timer;
};

void tcpExample()
{
	ExampleTcpServer &tcpServer = ExampleTcpServerSingleton::instance();
	if (!tcpServer.listen("127.0.0.1", 4567))
	{
		cout << "listen failed" << endl;
		return;
	}

	ExampleTcpClientManager clientManager;
	clientManager.startThread();
	clientManager.waitThread();
	tcpServer.wait();
}


