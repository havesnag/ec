/*
 * loopTest.cpp
 *
 *  Created on: 2015年6月27日
 *      Author: havesnag
 */


#include <iostream>
#include <functional>
using namespace std;

#include "ec/loop.h"
#include "ec/timer.h"
#include "ec/date.h"
#include "index.h"


class AsyncExampleServer
{
public:
	AsyncExampleServer() :
		_timer(_logicLoop)
	{
		_logicLoop.regAsyncHandler(1, std::bind(&AsyncExampleServer::doCommand1, this,
				placeholders::_1, placeholders::_2, placeholders::_3));
	}

	void start()
	{
		_timer.startRounds(1000, 2, std::bind(&AsyncExampleServer::tick, this));
		_eventLoop.startThread();
		_logicLoop.startThread();
		_eventLoop.waitThread();
		_logicLoop.waitThread();
	}

	void stop()
	{
		_eventLoop.stop();
		_logicLoop.stop();
	}

protected:
	void doCommand1(ec::Command cmd, const ec::Data &request, ec::Data &response)
	{
		uint64_t round = request.get<uint64_t>();
		cout << "round " << round << " doCommand1" << endl;
		round *= round;
		response.set(round);
	}

private:
	void tick()
	{
		uint64_t round = _timer.getCurRound();
		cout << "round " << round << " begin" << endl;

		_logicLoop.async(1, round,
				[=](ec::Command cmd, const ec::Data &request, ec::Data &response) {
			cout << "round " << round << " response=" << response.get<uint64_t>() << endl << endl;
		});

		cout << "round " << round << " return" << endl;

		if (_timer.isFinished())
		{
			stop();
		}
	}

private:
	ec::Loop _eventLoop;
	ec::Loop _logicLoop;
	ec::Timer _timer;
};

void asyncExample()
{
	AsyncExampleServer server;
	server.start();
	cout << "done" << endl;
}
