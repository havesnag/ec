/*
 * timer.cpp
 *
 *  Created on: 2015年6月29日
 *      Author: havesnag
 */

#include <iostream>
#include <functional>
using namespace std;

#include "ec/loop.h"
#include "ec/timer.h"
#include "index.h"

class ExampleTimerServer
{
public:
	ExampleTimerServer():
		_timer(_loop)
	{
	}

	void start()
	{
		_timer.startAfter(1000, 100, 2, [this]() {
			cout << "startAfter round " << _timer.getCurRound() << endl;
			if (_timer.isFinished())
			{
				_timer.startRounds(200, 2, std::bind(&ExampleTimerServer::tick, this));
			}
		});
		cout << "before start" << endl;
		_loop.start();
	}

	void stop()
	{
		_loop.stop();
	}

private:
	void tick()
	{
		uint64_t round = _timer.getCurRound();
		cout << "round " << round << " begin" << endl;
		if (_timer.isFinished())
		{
			stop();
		}
	}

private:
	ec::Loop _loop;
	ec::Timer _timer;
};

void timerExample()
{
	ExampleTimerServer server;
	server.start();
}
