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

template <typename T>
class Singleton : private T
{
private:
    Singleton();
    ~Singleton();

public:
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
