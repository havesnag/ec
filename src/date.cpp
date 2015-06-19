/*
 * date.cpp
 *
 *  Created on: 2015年6月7日
 *      Author: havesnag
 */

#include "ec/date.h"
#include <limits.h>
#include <sstream>
#include <iomanip>
using namespace std;

namespace ec
{

/***************************************************
 * class Date
 ***************************************************/

int Date::s_timeZoneOffset = INT_MIN;

int Date::getTimeZoneOffset()
{
	if (INT_MIN == s_timeZoneOffset)
	{
		Date utc(1970, 1, 1);
		s_timeZoneOffset = utc.stamp();
	}
	return s_timeZoneOffset;
}

Date::Date()
{
	time_t stamp = time(NULL);
	localtime_r(&stamp, &_tm);
}

Date::Date(time_t stamp)
{
	set(stamp);
}

Date::Date(const Time &time)
{
	set(time.stamp());
}

Date::Date(const Date &date)
{
	_tm = date._tm;
}

Date::Date(int year, int month, int day, int hour, int minute, int second)
{
	_tm.tm_year = year - 1900;
	_tm.tm_mon = month - 1;
	_tm.tm_mday = day;
	_tm.tm_hour = hour;
	_tm.tm_min = minute;
	_tm.tm_sec = second;
}

Date::~Date()
{
}

void Date::set(time_t stamp)
{
	localtime_r(&stamp, &_tm);
}

Date Date::clone() const
{
	return Date(*this);
}

time_t Date::stamp() const
{
	return mktime(const_cast<struct tm *>(&_tm));
}

time_t Date::utcStamp() const
{
	return stamp() + Date::getTimeZoneOffset();
}

Time Date::toTime() const
{
	return Time(stamp());
}

std::string Date::toString() const
{
	std::ostringstream oss;
	oss << setfill('0') << setw(4) << year()
		<< "-" << setw(2) << month()
		<< "-" << setw(2) << day()
		<< " " << setw(2) << hour()
		<< ":" << setw(2) << minute()
		<< ":" << setw(2) << second();
	return oss.str();
}

Date & Date::setYear(int year)
{
	_tm.tm_year = (year < 1900) ? 0 : year - 1900;
	return *this;
}

Date & Date::setMonth(int month)
{
	month = month % 13;
	_tm.tm_mon = (month > 0) ? month - 1 : 0;
	return *this;
}

Date & Date::setDay(int day)
{
	day = day%32;
	_tm.tm_mday = (day > 1) ? day : 1;
	return *this;
}

Date & Date::setHour(int hour)
{
	hour = hour%24;
	_tm.tm_hour = (hour > 0) ? hour : 0;
	return *this;
}

Date & Date::setMinute(int minute)
{
	minute = minute%60;
	_tm.tm_min = (minute > 0) ? minute : 0;
	return *this;
}

Date & Date::setSecond(int second)
{
	second = second%60;
	_tm.tm_sec = (second > 0) ? second : 0;
	return *this;
}


Date & Date::addYear(int year)
{
	_tm.tm_year += year;
	if (_tm.tm_year < 0)
	{
		_tm.tm_year = 0;
	}

	int monthDays = getMonthDays();
	if (_tm.tm_mday > monthDays)
	{
		_tm.tm_mday = monthDays;
	}

	return *this;
}

Date & Date::addMonth(int month)
{
	_tm.tm_mon = _tm.tm_mon + month;
	_tm.tm_year = _tm.tm_year + _tm.tm_mon/12;
	_tm.tm_mon = _tm.tm_mon % 12;

	int monthDays = getMonthDays();
	if (_tm.tm_mday > monthDays)
	{
		_tm.tm_mday = monthDays;
	}

	return *this;
}

Date & Date::addSecond(int second)
{
	Time time(stamp());
	time.addSecond(second);
	set(time.stamp());
	return *this;
}

Date & Date::sharpYear()
{
	_tm.tm_mon = 0;
	_tm.tm_mday = 1;
	_tm.tm_hour = 0;
	_tm.tm_min = 0;
	_tm.tm_sec = 0;
	return *this;
}

Date & Date::sharpMonth()
{
	_tm.tm_mday = 1;
	_tm.tm_hour = 0;
	_tm.tm_min = 0;
	_tm.tm_sec = 0;
	return *this;
}

Date & Date::sharpWeek()
{
	sharpDay();
	addSecond((1 - week()) * Time::DAY_SECONDS);
	return *this;
}

Date & Date::sharpDay()
{
	_tm.tm_hour = 0;
	_tm.tm_min = 0;
	_tm.tm_sec = 0;
	return *this;
}

Date & Date::sharpHour()
{
	_tm.tm_min = 0;
	_tm.tm_sec = 0;
	return *this;
}

Date & Date::sharpMinute()
{
	_tm.tm_sec = 0;
	return *this;
}

bool Date::isLeapYear() const
{
	int curYear = year();
	return (curYear%4 == 0 && curYear%100 != 0);
}

bool Date::isEndOfMonth() const
{
	int curDay = day();
	return curDay >= getMonthDays();
}

int Date::getMonthDays() const
{
	int curMonth = month();
	switch (curMonth)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	case 2:
		//Need to consider a leap year in February
		return isLeapYear() ? 29 : 28;
	default:
		return 0;
	}
}



/***************************************************
 * class Time
 ***************************************************/
Time::Time()
{
	gettimeofday(&_tv, NULL);
}

Time::Time(time_t stamp)
{
	set(stamp);
}

Time::Time(const Date &date)
{
	set(date.stamp());
}

Time::Time(const Time &time)
{
	_tv = time._tv;
}

Time::~Time()
{
}

void Time::set(time_t stamp)
{
	_tv.tv_sec = stamp;
	_tv.tv_usec = 0;
}

Time Time::clone() const
{
	return Time(*this);
}

time_t Time::utcStamp() const
{
	return stamp() + Date::getTimeZoneOffset();
}

Date Time::toDate() const
{
	return Date(stamp());
}

Time & Time::addSecond(int second)
{
	_tv.tv_sec += second;
	return *this;
}

Time & Time::sharpHour()
{
	_tv.tv_sec = _tv.tv_sec/3600 * 3600;
	_tv.tv_usec = 0;
	return *this;
}

Time & Time::sharpMinute()
{
	_tv.tv_sec = _tv.tv_sec/60 * 60;
	_tv.tv_usec = 0;
	return *this;
}

Time & Time::nextPeriod(Time::PeriodType type, int times)
{
	switch (type)
	{
	case kPeriodMinute:
		addSecond(Time::MINUTE_SECONDS * times);
		break;
	case kPeriodHour:
		addSecond(Time::HOUR_SECONDS * times);
		break;
	case kPeriodDay:
		addSecond(Time::DAY_SECONDS * times);
		break;
	case kPeriodWeek:
		addSecond(Time::WEEK_SECONDS * times);
		break;
	case kPeriodMonth:
		set(toDate().addMonth(times).stamp());
		break;
	case kPeriodYear:
		set(toDate().addYear(times).stamp());
		break;
	default:
		break;
	}

	return *this;
}

int Time::getFullDays() const
{
	return (utcStamp()/Time::DAY_SECONDS);
}


} /* namespace ec */
