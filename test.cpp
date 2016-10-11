//============================================================================
// Name        : test.cpp
// Author      : havesnag
// Version     :
// Copyright   : xiaceng.com
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <string>
#include <map>
using namespace std;

#include "example/index.h"
#include "ec/define.h"

int main(int argc, char *argv[])
{
	string cmd = "timer";
	if (argc > 1) {
		cmd = argv[1];
	}

	map<string, ec::Handler> handlers;
	handlers["timer"] = std::bind(&timerExample);
	handlers["tcp"] = std::bind(&tcpExample);
	handlers["http"] = std::bind(&httpExample);

	ec::Handler handler = handlers[cmd];
	if (!handler)
	{
		cout << "invalid handler" << endl;
		return 0;
	}

	handler();
	return 0;
}
