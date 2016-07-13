/*
 * tcpSessionFactory.h
 *
 *  Created on: 2016年7月12日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_TCPSESSIONFACTORY_H_
#define INCLUDE_EC_TCPSESSIONFACTORY_H_

#include "ec/tcpSession.h"

namespace ec
{
class TcpSessionFactory
{
public:
	TcpSessionFactory() {};
	virtual ~TcpSessionFactory() {};

	virtual TcpSessionPtr create()
	{
		return TcpSessionPtr(new TcpSession());
	}
};

} /* namespace ec */



#endif /* INCLUDE_EC_TCPSESSIONFACTORY_H_ */
