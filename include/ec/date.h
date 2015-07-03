/*
 * date.h
 *
 *  Created on: 2015年6月7日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_DATE_H_
#define INCLUDE_EC_DATE_H_

#include <time.h>
#include <stdint.h>
#include <string>

#if (defined _WIN32) || (defined WIN32) || (defined _WIN64) || (defined WIN64)
#define localtime_r(t, tm) localtime_s(tm, t)
#else
#include <sys/time.h>
#endif//_WIN32

namespace ec
{

class Time;
class Date;

/**
 * @brief 日期类
 */
class Date
{
private:
	static int s_timeZoneOffset;

public:
	/** 返回当前时区，比如UTC+8的时区为-28800 */
	static int getTimeZoneOffset();

public:
	/** 以当前时间构造 */
	Date();
	/** 以时间戳构造 */
	Date(time_t stamp);
	/** 以Time对象构造 */
	Date(const Time &time);
	Date(const Date &date);

	/**
	 * @brief 以指定时间构造
	 * @param year 年，[1900)
	 * @param month 月，[1,12]
	 * @param day 日，[1,31]
	 * @param hour 时，[0,23]，默认为0
	 * @param minute 分，[0,59]，默认为0
	 * @param second 秒，[0,60]，默认为0
	 */
	Date(int year, int month, int day, int hour = 0, int minute = 0, int second = 0);

	~Date();

	/** 设置时间戳 */
	void set(time_t stamp);

	/** 克隆当前对象 */
	Date clone() const;

	/** 获取时间戳 */
	time_t stamp() const;

	/** 获取UTC时间戳 */
	time_t utcStamp() const;

	/** 转换为Time */
	Time toTime() const;

	/** 转换为字符串 */
	std::string toString() const;

	/** 获取年，[1900, ) */
	inline int year() const
	{
		return _tm.tm_year + 1900;
	}

	/** 获取月，[1,12] */
	inline int month() const
	{
		return _tm.tm_mon + 1;
	}

	/** 获取日，[1,31] */
	inline int day() const
	{
		return _tm.tm_mday;
	}

	/** 获取时，[0,23] */
	inline int hour() const
	{
		return _tm.tm_hour;
	}

	/** 获取分，[0,59] */
	inline int minute() const
	{
		return _tm.tm_min;
	}

	/** 获取秒，[0,60] */
	inline int second() const
	{
		return _tm.tm_sec;
	}

	/** 获取星期，[1,7] */
	inline int week() const
	{
		return (_tm.tm_wday > 0) ? _tm.tm_wday : 7;
	}

	/** 获取一年中的天，[0,365] */
	inline int yearDay() const
	{
		return _tm.tm_yday;
	}


	/** 设置年，[1900, ) */
	Date & setYear(int year);
	/** 设置月，[1,12] */
	Date & setMonth(int month);
	/** 设置日，[1,31] */
	Date & setDay(int day);
	/** 设置时，[0,23] */
	Date & setHour(int hour);
	/** 设置分，[0,59] */
	Date & setMinute(int minute);
	/** 设置秒，[0,60] */
	Date & setSecond(int second);

	/** 加/减 年 */
	Date & addYear(int year);
	/** 加/减 月 */
	Date & addMonth(int month);
	/** 加/减 秒 */
	Date & addSecond(int second);

	/** 设置为一年的开始 */
	Date & sharpYear();
	/** 设置为一月的开始 */
	Date & sharpMonth();
	/** 设置为一周的开始 */
	Date & sharpWeek();
	/** 设置为一天的开始 */
	Date & sharpDay();
	/** 设置为一小时的开始 */
	Date & sharpHour();
	/** 设置为一分钟的开始 */
	Date & sharpMinute();

	/** 是否是闰年 */
	bool isLeapYear() const;
	/** 是否是一月的最后一天 */
	bool isEndOfMonth() const;
	/** 获取在本月中的天数 */
	int getMonthDays() const;

private:
	struct tm _tm;
};

/**
 * @brief 时间类
 */
class Time
{
public:
	/** 每秒包含毫秒数 */
	static const time_t SECOND_MS = 1000;
	/** 每分钟包含秒数 */
	static const time_t MINUTE_SECONDS = 60;
	/** 每小时包含秒数 */
	static const time_t HOUR_SECONDS = 3600;
	/** 每天包含秒数 */
	static const time_t DAY_SECONDS = HOUR_SECONDS*24;
	/** 每周包含秒数 */
	static const time_t WEEK_SECONDS = DAY_SECONDS*7;

	/** 时间周期类型枚举 */
	enum PeriodType
	{
		kPeriodNone = 0, /** 无效的周期 */
		kPeriodMinute, /** 周期 分 */
		kPeriodHour, /** 周期 小时 */
		kPeriodDay, /** 周期 天 */
		kPeriodWeek, /** 周期 周 */
		kPeriodMonth, /** 周期 月 */
		kPeriodYear, /** 周期 年 */
		kPeriodMax
	};

public:
	Time();
	/** 以时间戳构造 */
	Time(time_t stamp);
	/** 以Date对象构造 */
	Time(const Date &date);
	Time(const Time &time);
	~Time();

	/** 设置时间戳 */
	void set(time_t stamp);

	/** 克隆当前对象 */
	Time clone() const;

	/** 获取时间戳 */
	inline time_t stamp() const
	{
		return _tv.tv_sec;
	}

	/** 获取UTC时间戳 */
	time_t utcStamp() const;

	/** 转换为Date对象 */
	Date toDate() const;

	/** 获取毫秒时间戳 */
	inline int64_t ms() const
	{
		return _tv.tv_sec*1000 + _tv.tv_usec/1000;
	}

	/** 加/减 秒 */
	Time & addSecond(int second);

	/** 设置为一小时的开始 */
	Time & sharpHour();
	/** 设置为一分钟的开始 */
	Time & sharpMinute();

	/** 到下一周期 */
	Time & nextPeriod(Time::PeriodType type, int times);

	/** 距离1970-01-01 00:00:00的天数 */
	int getFullDays() const;

	/** 计算经过的天数 */
	int getDayPassed(const Time &refTime);

private:
	struct timeval _tv;
};

} /* namespace ec */

#endif /* INCLUDE_EC_DATE_H_ */
