/*
 * tcpSocket.h
 *
 *  Created on: 2015年6月24日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_TCPSOCKET_H_
#define INCLUDE_EC_TCPSOCKET_H_

#include <atomic>
#include <event2/bufferevent.h>
#include "ec/define.h"

namespace ec
{

class TcpSocket
{
public:
	TcpSocket();
	virtual ~TcpSocket();

	inline bool isInited() const
	{
		return (NULL != _bev);
	}

	SocketFd getSocket() const;
	void getAddr(struct sockaddr_in * dest, uint32_t size) const;

	//发送数据
	bool send(const char *data, uint32_t size);

	//关闭连接
	void close(bool pending = true);

	//获取收数据缓冲区数据长度
	uint32_t getInputBufferLength() const;
	//查看收数据缓冲区
	const uint8_t * viewInputBuffer(uint32_t size) const;
	//读取收数据缓冲区
	bool readInputBuffer(uint8_t * dest, uint32_t size);
	//清空收数据缓冲区
	void clearInputBuffer();

protected:
	//子类需要调用此接口检查正在关闭的数据
	void checkClosing();

private:
	void closeImpl();

protected:
	struct bufferevent *_bev;

private:
	std::atomic<bool> _isClosing;
};

} /* namespace ec */



#endif /* INCLUDE_EC_TCPSOCKET_H_ */
