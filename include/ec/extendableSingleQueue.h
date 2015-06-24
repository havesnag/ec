/*
 * extendableSingleQueue.h
 *
 *  Created on: 2015年6月22日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_EXTENDABLESINGLEQUEUE_H_
#define INCLUDE_EC_EXTENDABLESINGLEQUEUE_H_

#include <queue>
#include "ec/singleQueue.h"

namespace ec {

/*
 * @desc	:可自动扩容的单向队列，一个线程写入，另一线程读取是安全的。
 */
template <typename T>
class ExtendableSingleQueue
{
public:
    explicit ExtendableSingleQueue(uint32_t capacity) :
		_singleQueue(capacity)
	{}

    void push(T value)
	{
		if (_singleQueue.full())
		{
			_extendQueue.push(value);
		}
		else
		{
			if (_extendQueue.empty())
			{
				_singleQueue.push(value);
			}
			else
			{
				_extendQueue.push(value);
				refresh();
			}
		}
	}

    void refresh()
    {
    	for (; !_extendQueue.empty() && !_singleQueue.full(); )
		{
			_singleQueue.push(_extendQueue.front());
			_extendQueue.pop();
		}
    }

	void pop()
	{
		_singleQueue.pop();
	}

	bool empty()
	{
		return _singleQueue.empty();
	}

	uint32_t count()
	{
		return _singleQueue.count() + _extendQueue.size();
	}

	T & front()
	{
		return _singleQueue.front();
	}

private:
    ec::SingleQueue<T> _singleQueue;
    std::queue<T> _extendQueue;
};


} /* namespace ec */


#endif /* INCLUDE_EC_EXTENDABLESINGLEQUEUE_H_ */
