/*
 * singleQueue.h
 *
 *  Created on: 2015年6月22日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_SINGLEQUEUE_H_
#define INCLUDE_EC_SINGLEQUEUE_H_

#include <queue>

namespace ec
{

/**
 * @brief 单循环队列
 * @details 一个线程写入，另一线程读取是安全的
 */
template <typename T>
class SingleQueue
{
public:
	/**
	 * @brief 构造函数
	 * @param capacity 队列容量
	 */
	SingleQueue(uint32_t capacity) :
		_capacity(capacity + 1),
		_frontPos(0),
		_rearPos(0)
	{
		_elements = new T[_capacity];
	}

	~SingleQueue()
	{
		delete [] _elements;
	}

	/** 加一个元素到队尾 */
	void push(T value)
	{
	   _elements[_rearPos] = value;
	   _rearPos = (_rearPos + 1) % _capacity;
	}

	/** 移除队头元素 */
	void pop()
	{
		_frontPos = (_frontPos + 1) % _capacity;
	}

	/** 是否为空 */
	bool empty()
	{
		return _frontPos == _rearPos;
	}

	/** 是否已满 */
	bool full()
	{
		return (_rearPos + 1) % _capacity == _frontPos;
	}

	/** 获取元素数量 */
	uint32_t count()
	{

		uint32_t count = 0;
		if (_frontPos <= _rearPos)
		{
			count = _rearPos - _frontPos;
		}
		else
		{
			count = _capacity + (_rearPos - _frontPos);
		}

		return count;
	}

	/** 获取对头元素 */
	T & front()
	{
		return _elements[_frontPos];
	}

	/** 获取队尾元素 */
	T & rear()
	{
		return _elements[_rearPos == 0 ? _capacity - 1 : _rearPos - 1];
	}

private:
    T * _elements;
    uint32_t _capacity;
    volatile uint32_t _frontPos;
    volatile uint32_t _rearPos;
};

} /* namespace ec */


#endif /* INCLUDE_EC_SINGLEQUEUE_H_ */
