/*
 * safeQueue.h
 *
 *  Created on: 2015年6月22日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_SAFEQUEUE_H_
#define INCLUDE_EC_SAFEQUEUE_H_

#include <list>
#include <mutex>

namespace ec
{

template<typename T>
class SafeQueue
{
public:
	void push(T t)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_elements.push_back(t);
	}

	std::list<T> moveAll()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		std::list<T> elements = _elements;
		_elements.clear();
		return elements;
	}

	void moveAll(std::list<T> &elements)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		elements = _elements;
		_elements.clear();
	}

	bool empty()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _elements.empty();
	}

	size_t size()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _elements.size();
	}

private:
	std::list<T> _elements;
	std::mutex _mutex;
};

} /* namespace ec */


#endif /* INCLUDE_EC_SAFEQUEUE_H_ */
