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

/**
 * @brief 可自动扩容的单向队列
 * @details 一个线程写入，另一线程读取是安全的;
 * @details 内部维护了一个单循环队列和一个普通队列，当单循环队列队列满了后会启用普通队列保存元素
 */
template <typename T>
class ExtendableSingleQueue
{
public:
    explicit ExtendableSingleQueue(uint32_t capacity) :
		_singleQueue(capacity)
	{}

    /** 加一个元素到队尾 */
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

    /** 刷新数据 */
    void refresh()
    {
    	for (; !_extendQueue.empty() && !_singleQueue.full(); )
		{
			_singleQueue.push(_extendQueue.front());
			_extendQueue.pop();
		}
    }

    /** 移除队头元素 */
	void pop()
	{
		_singleQueue.pop();
	}

	/** 是否为空 */
	bool empty()
	{
		return _singleQueue.empty();
	}

	/** 获取元素数量 */
	uint32_t count()
	{
		return _singleQueue.count() + _extendQueue.size();
	}

	/** 获取队尾元素 */
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
