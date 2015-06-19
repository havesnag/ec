/*
 * date.h
 *
 *  Created on: 2015年6月7日
 *      Author: havesnag
 */

#ifndef EC_DATE_H_
#define EC_DATE_H_

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

class Date
{
private:
	static int s_timeZoneOffset;
public:
	static int getTimeZoneOffset();

public:
	Date();
	Date(time_t stamp);
	Date(const Time &time);
	Date(const Date &date);

	//以指定时间构造
	Date(int year,			/* 年，[1900) */
			int month,		/* 月，[1,12] */
			int day, 		/* 日，[1,31] */
			int hour = 0, 	/* 时，[0,23] */
			int minute = 0, /* 分，[0,59] */
			int second = 0 	/* 秒，[0,60] */);

	~Date();

	//从时间戳设置
	void set(time_t stamp);

	//克隆
	Date clone() const;

	//时间戳
	time_t stamp() const;

	//UTC时间戳
	time_t utcStamp() const;

	//转换为Time
	Time toTime() const;

	//转换为字符串
	std::string toString() const;


	//年，[1900, )
	inline int year() const
	{
		return _tm.tm_year + 1900;
	}

	//月，[1,12]
	inline int month() const
	{
		return _tm.tm_mon + 1;
	}

	//日，[1,31]
	inline int day() const
	{
		return _tm.tm_mday;
	}

	//时，[0,23]
	inline int hour() const
	{
		return _tm.tm_hour;
	}

	//分，[0,59]
	inline int minute() const
	{
		return _tm.tm_min;
	}

	//秒，[0,60]
	inline int second() const
	{
		return _tm.tm_sec;
	}

	//星期，[1,7]
	inline int week() const
	{
		return (_tm.tm_wday > 0) ? _tm.tm_wday : 7;
	}

	//一年中的天，[0,365]
	inline int yearDay() const
	{
		return _tm.tm_yday;
	}


	//设置年，[1900, )
	Date & setYear(int year);
	//设置月，[1,12]
	Date & setMonth(int month);
	//设置日，[1,31]
	Date & setDay(int day);
	//设置时，[0,23]
	Date & setHour(int hour);
	//设置分，[0,59]
	Date & setMinute(int minute);
	//设置秒，[0,60]
	Date & setSecond(int second);

	//加/减 年
	Date & addYear(int year);
	//加/减 月
	Date & addMonth(int month);
	//加/减 秒
	Date & addSecond(int second);

	//设置为一年的开始
	Date & sharpYear();
	//设置为一月的开始
	Date & sharpMonth();
	//设置为一周的开始
	Date & sharpWeek();
	//设置为一天的开始
	Date & sharpDay();
	//设置为一小时的开始
	Date & sharpHour();
	//设置为一分钟的开始
	Date & sharpMinute();

	//是否是闰年
	bool isLeapYear() const;
	//是否是一月的最后一天
	bool isEndOfMonth() const;
	//本月总天数
	int getMonthDays() const;

private:
	struct tm _tm;


};

class Time
{
public:
	static const time_t SECOND_MS = 1000;
	static const time_t MINUTE_SECONDS = 60;
	static const time_t HOUR_SECONDS = 3600;
	static const time_t DAY_SECONDS = HOUR_SECONDS*24;
	static const time_t WEEK_SECONDS = DAY_SECONDS*7;

	enum PeriodType
	{
		kPeriodNone = 0,
		kPeriodMinute,
		kPeriodHour,
		kPeriodDay,
		kPeriodWeek,
		kPeriodMonth,
		kPeriodYear,
		kPeriodMax
	};

public:
	Time();
	Time(time_t stamp);
	Time(const Date &date);
	Time(const Time &time);
	~Time();

	//从时间戳设置
	void set(time_t stamp);

	//克隆
	Time clone() const;

	//时间戳
	inline time_t stamp() const
	{
		return _tv.tv_sec;
	}

	//UTC时间戳
	time_t utcStamp() const;

	//转换为Date对象
	Date toDate() const;

	//毫秒时间戳
	inline int64_t ms() const
	{
		return _tv.tv_sec*1000 + _tv.tv_usec/1000;
	}

	//加/减 秒
	Time & addSecond(int second);

	//设置为一小时的开始
	Time & sharpHour();
	//设置为一分钟的开始
	Time & sharpMinute();

	//到下一周期
	Time & nextPeriod(Time::PeriodType type, int times);

	//距离1970-01-01 00:00:00的天数
	int getFullDays() const;

	//
	int getDayPassed(const Time &refTime);


private:
	struct timeval _tv;
};

} /* namespace ec */

#endif /* EC_DATE_H_ */
