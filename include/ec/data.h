/*
 * data.h
 *
 *  Created on: 2015年6月23日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_DATA_H_
#define INCLUDE_EC_DATA_H_

#include "ec/define.h"

namespace ec
{

class Data
{
public:

	Data()
	{
		reset();
	}

	Data(Data &data)
	{
		_data = data._data;
		_dataSize = data._dataSize;
		data._data = NULL;
		data._dataSize = 0;
	}

	Data(const uint8_t *data, uint16_t dataSize)
	{
		set(data, dataSize);
	}

	inline const uint8_t * data() const
	{
		return _data;
	}

	inline uint16_t dataSize() const
	{
		return _dataSize;
	}

	void reset()
	{
		if (NULL != _data)
		{
			delete [] _data;
		}

		_data = NULL;
		_dataSize = 0;
	}

	void set(const uint8_t *data, uint16_t dataSize)
	{
		if (NULL != data && dataSize > 0)
		{
			reset();
			_data = new uint8_t[dataSize];
			_dataSize = dataSize;
			memcpy(_data, data, dataSize);
		}
	}

	template <typename T>
	void set(T &value)
	{
		reset();
		set((uint8_t *)(&value), sizeof(value));
	}

	template <typename T>
	T get() const
	{
		T value;
		memcpy(&value, _data, sizeof(T));
		return value;
	}

private:
	uint8_t * _data;
	uint16_t _dataSize;
};

} /* namespace ec */

#endif /* INCLUDE_EC_DATA_H_ */
