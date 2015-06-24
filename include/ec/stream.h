/*
 * stream.h
 *
 *  Created on: 2015年6月24日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_STREAM_H_
#define INCLUDE_EC_STREAM_H_

#include "ec/define.h"
#include <stdio.h>
#include <memory.h>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <string>





class StreamWriter
{
public:
	StreamWriter(uint32_t size = 128)
	{
		_buf.reserve(size);
	}

	inline operator uint8_t * () const
	{
		return _buf.empty() ? NULL : (uint8_t *)&_buf[0];
	}

	inline const uint8_t * operator & () const
	{
		return _buf.empty() ? NULL : &_buf[0];
	}

	inline uint32_t size() const
	{
		return _buf.size();
	}

	inline void put(const uint8_t *data, uint32_t dataLen)
	{
		_buf.insert(_buf.end(), data, data + dataLen);
	}

	template<typename T>
	StreamWriter & put (const T &v)
	{
		put((uint8_t *)&v, sizeof(T));
		return *this;
	}

	template<typename T>
	StreamWriter & operator << (const T &v)
	{
		put((uint8_t *)&v, sizeof(T));
		return *this;
	}

	StreamWriter & operator << (const std::string &v)
	{
		(*this) << static_cast<uint16_t>(v.length());
		_buf.insert(_buf.end(), v.begin(), v.end());
		return *this;
	}

	template<typename T>
	inline StreamWriter & operator << (const std::vector<T> &values)
	{
		uint16_t itemCount = static_cast<uint16_t>(values.size());
		(*this) << itemCount;
		if (itemCount > 0)
		{
			put((uint8_t *)&values[0], itemCount*sizeof(T));
		}
		return *this;
	}

	template<typename T>
	inline StreamWriter & operator << (const std::list<T> &values)
	{
		uint16_t itemCount = static_cast<uint16_t>(values.size());
		(*this) << itemCount;
		for (auto value : values)
		{
			(*this) << value;
		}
		return *this;
	}

	template<typename T>
	inline StreamWriter & operator << (const std::set<T> &values)
	{
		uint16_t itemCount = static_cast<uint16_t>(values.size());
		(*this) << itemCount;
		for (auto value : values)
		{
			(*this) << value;
		}
		return *this;
	}

	template<typename KeyT, typename ValueT>
	inline StreamWriter & operator << (const std::map<KeyT, ValueT> &values)
	{
		uint16_t itemCount = static_cast<uint16_t>(values.size());
		(*this) << itemCount;
		for (auto value : values)
		{
			(*this) << value.first;
			(*this) << value.second;
		}
		return *this;
	}
	
	template <typename SequenceT>
	inline StreamWriter & putSequence(const SequenceT &values)
	{
		uint16_t itemCount = static_cast<uint16_t>(values.size());
		(*this) << itemCount;
		for (auto value : values)
		{
			(*this) << value;
		}

		return *this;
	}

protected:
	std::vector<uint8_t> _buf;
};


class StreamReader
{
public:
	StreamReader(const uint8_t *buf, uint32_t bufLength) :
		_data(buf),
		_dataSize(bufLength),
		_pos(0)
	{
	}

	inline operator uint8_t * ()
	{
		return (uint8_t *)_data;
	}

	inline const uint8_t * operator & () const
	{
		return _data;
	}

	inline uint32_t size() const
	{
		return _dataSize;
	}

	uint32_t left() const
	{
		return _dataSize - _pos;
	}

	uint32_t pos() const
	{
		return _pos;
	}

	uint32_t get(uint8_t * dataBuffer, uint32_t dataLen)
	{
		if(_pos + dataLen > _dataSize)
		{
			return 0;
		}

		memcpy(dataBuffer, &_data[_pos], dataLen);
		_pos += dataLen;

		return dataLen;
	}

	template<typename T>
	inline StreamReader & get (T &v)
	{
		get((uint8_t *)&v, sizeof(T));
		return *this;
	}

	template<typename T>
	inline StreamReader & operator >> (T &v)
	{
		get((uint8_t *)&v, sizeof(T));
		return *this;
	}

	StreamReader & operator >> (std::string &v)
	{
		uint16_t strLen;
		(*this) >> strLen;
		uint32_t leftSize = left();

		if (strLen > leftSize)
		{
			strLen = leftSize;
		}

		if (strLen > 0)
		{
			v.resize(strLen);
			memcpy(&v[0], &_data[_pos], strLen);
			_pos += strLen;
		}

		return *this;
	}

	template<typename T>
	inline StreamReader & operator >> (std::vector<T> &values)
	{
		uint16_t itemCount;
		(*this) >> itemCount;
		uint32_t leftSize = left();
		uint32_t needSize = itemCount*sizeof(T);
		if (leftSize < needSize)
		{
			itemCount = leftSize/sizeof(T);
			needSize = itemCount*sizeof(T);
		}

		if (itemCount > 0)
		{
			values.resize(itemCount);
			memcpy((uint8_t *)&values[0], &_data[_pos], needSize);
			_pos += needSize;
		}

		return *this;
	}

	template<typename T>
	inline StreamReader & operator >> (std::list<T> &values)
	{
		uint16_t itemCount = 0;
		(*this) >> itemCount;
		for (uint16_t i = 0; i < itemCount; ++ i)
		{
			T value;
			(*this) >> value;
			values.push_back(value);
		}

		return *this;
	}

	template<typename T>
	inline StreamReader & operator >> (std::set<T> &values)
	{
		uint16_t itemCount = 0;
		(*this) >> itemCount;
		for (uint16_t i = 0; i < itemCount; ++ i)
		{
			T value;
			(*this) >> value;
			values.insert(value);
		}

		return *this;
	}

	template<typename KeyT, typename ValueT>
	inline StreamReader & operator >> (std::map<KeyT, ValueT> &values)
	{
		uint16_t itemCount = 0;
		(*this) >> itemCount;
		for (uint16_t i = 0; i < itemCount; ++ i)
		{
			typename std::pair<KeyT, ValueT> pair;
			(*this) >> pair.first;
			(*this) >> pair.second;
			values.insert(pair);
		}

		return *this;
	}

	template <typename SequenceT>
	StreamReader & getSequence(SequenceT &values)
	{
		uint16_t itemCount;
		(*this) >> itemCount;
		uint32_t leftSize = left();
		uint16_t needSize = itemCount*sizeof(typename SequenceT::value_type);
		if (needSize > leftSize)
		{
			itemCount = leftSize/sizeof(typename SequenceT::value_type);
		}

		for (uint16_t i = 0; i < itemCount; ++i)
		{
			typename SequenceT::value_type value;
			(*this) >> value;
			values.insert(values.end(), value);
		}

		return *this;
	}

private:
	const uint8_t *_data;
	uint32_t _dataSize;
	uint32_t _pos;
};

#endif /* INCLUDE_EC_STREAM_H_ */
