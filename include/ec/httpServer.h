/*
 * httpServer.h
 *
 *  Created on: 2015年7月7日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_HTTPSERVER_H_
#define INCLUDE_EC_HTTPSERVER_H_

#include <string>
#include <map>
#include "ec/loop.h"
#include "ec/httpRequest.h"

namespace ec
{

typedef std::function<void (ec::HttpRequest *request)> HttpHandler;

class HttpServer : public ec::Loop
{
public:
	HttpServer();
	virtual ~HttpServer();

	bool listen(const char *ip, uint16_t port);

	void regHandler(const char * path, ec::HttpHandler handler);

private:
	static void genericCallback(struct evhttp_request *req, void *arg);
	static void mainCallback(struct evhttp_request *req, void *arg);

private:
	std::string _ip;
	uint16_t _port;

	struct evhttp *_http;

	std::map<std::string, ec::HttpHandler> _handlers;
};

} /* namespace ec */

#endif /* INCLUDE_EC_HTTPSERVER_H_ */
