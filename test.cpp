//============================================================================
// Name        : test.cpp
// Author      : havesnag
// Version     :
// Copyright   : xiaceng.com
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <functional>
using namespace std;

#include "ec/loop.h"
#include "ec/timer.h"
#include "ec/date.h"

using namespace ec;


ec::Loop loop1;
ec::Loop loop2;

void doSome(int v)
{
	cout << v << endl;
}

void doCommand1(ec::Command cmd, ec::Data &data)
{
	cout << "doCommand1" << endl;
	int result = 20;
	data.set(result);
}

void request()
{
	cout << "do request" << endl;

	int value = 10;
	loop2.call(1, NULL, 0, [=](ec::Command cmd, ec::Data &data) {
		cout << value + data.get<int32_t>() << endl;
	});

	cout << "finished" << endl;
}



int main()
{
	loop2.regCommandHandler(1, doCommand1);

	ec::Timer timer(loop1);
	timer.startOnce(2000, request);

	loop1.startThread();
	loop2.startThread();

	loop1.waitThread();
	loop2.waitThread();

	return 0;
}
