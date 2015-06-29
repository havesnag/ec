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
public:
	ExampleTcpServer()
	{
		_client = new ExampleTcpClient(getLoop());
		_timer = new ec::Timer(getLoop());
		_timer->startForever(1000, std::bind(&ExampleTcpServer::tick, this));
	}

	~ExampleTcpServer()
	{
		delete _client;
		delete _timer;
	}

protected:
	virtual void onListenError()
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
	}

	virtual void onSessionRead(ec::TcpSession *session)
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
		uint32_t length = session->getInputBufferLength();
		char * buff = new char[length + 1];
		session->readInputBuffer((uint8_t *)buff, length);
		buff[length] = '\0';
		cout << "ExampleTcpServer::" << __FUNCTION__ << " " << buff << endl;
	}

	virtual void onSessionDisconnected(ec::TcpSession *session)
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
	}

	virtual void onNewSession(ec::TcpSession *session)
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
	}

private:
	void tick()
	{
		cout << "round " << _timer->getCurRound() << endl;
		if (!_client->isConnected())
		{
			_client->connect("127.0.0.1", 4567);
		}

		if (_timer->getCurRound() >= 5)
		{
			stop();
			return;
		}

		if (_timer->getCurRound() >= 3)
		{
			_client->close();
		}


	}

private:
	ExampleTcpClient *_client;
	ec::Timer *_timer;
};


void tcpExample()
{
	ExampleTcpServer tcpServer;
	tcpServer.listen("127.0.0.1", 4567);
	tcpServer.start();
}


