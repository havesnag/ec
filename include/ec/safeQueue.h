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

/**
 * @brief 线程安全队列
 */
template<typename T>
class SafeQueue
{
public:
	/** 加一个元素到队尾 */
	void push(T t)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_elements.push_back(t);
	}

	/**
	 * @brief 移除所有元素
	 * @return 移除的元素列表
	 */
	std::list<T> moveAll()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		std::list<T> elements = _elements;
		_elements.clear();
		return elements;
	}

	/**
	 * @brief 移除所有元素并添加到指定列表队尾中
	 * @param elements 存储移除的元素
	 */
	void moveAll(std::list<T> &elements)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		elements = _elements;
		_elements.clear();
	}

	/** 是否为空 */
	bool empty()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _elements.empty();
	}

	/** 获取元素个数 */
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
