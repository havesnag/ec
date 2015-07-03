/*
 * singleton.h
 *
 *  Created on: 2015年6月27日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_SINGLETON_H_
#define INCLUDE_EC_SINGLETON_H_

namespace ec
{

/**
 * @brief 单件类
 * @details 继承此类实现单件
 */
template <typename T>
class Singleton : private T
{
private:
    Singleton();
    ~Singleton();

public:
    /** 获取单件实例 */
    static T &instance();
};


template <typename T>
inline Singleton<T>::Singleton()
{}

template <typename T>
inline Singleton<T>::~Singleton()
{}

template <class T>
T &Singleton<T>::instance()
{
    static Singleton<T> ins;
    return ins;
}

} /* namespace ec */


#endif /* INCLUDE_EC_SINGLETON_H_ */
