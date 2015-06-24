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

/*
 * @desc	:单向队列，一个线程写入，另一线程读取是安全的。
 */
template <typename T>
class SingleQueue
{
public:
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

	void push(T value)
	{
	   _elements[_rearPos] = value;
	   _rearPos = (_rearPos + 1) % _capacity;
	}

	void pop()
	{
		_frontPos = (_frontPos + 1) % _capacity;
	}

	bool empty()
	{
		return _frontPos == _rearPos;
	}

	bool full()
	{
		return (_rearPos + 1) % _capacity == _frontPos;
	}

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

	T & front()
	{
		return _elements[_frontPos];
	}

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
