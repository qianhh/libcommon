#include "StdAfx.h"
#include "Date.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

namespace eric { namespace common {
// Constructor that initializes the object to a day, month and year
Date::Date(int nYear, int nMonth, int nDay) : m_nDay(nDay), m_nMonth(nMonth), m_nYear(nYear)
{
}

Date::Date(void) : m_nDay(0), m_nMonth(0), m_nYear(0)
{
	SYSTEMTIME systime = {0};
	::GetLocalTime(&systime);
	m_nYear = systime.wYear;
	m_nMonth = systime.wMonth;
	m_nDay = systime.wDay;
}

//日期加天数，因为定义的天数为int型，所以可能加一个负的天数
//故在循环中添加了判断处理，若负则执行减操作，反之执行加操作 
void Date::AddDays(int nDaysToAdd)
{
	m_nDay += nDaysToAdd;
	while (m_nDay > GetMonthDay(m_nYear, m_nMonth) || m_nDay <= 0)
	{ 
		if (m_nDay > 0)
		{
			m_nDay -= GetMonthDay(m_nYear, m_nMonth);
			if (m_nMonth == 12)
			{
				m_nMonth = 0;
				++m_nYear;
			}
			++m_nMonth;
		} 
		else
		{ 
			m_nDay += GetMonthDay(m_nYear, m_nMonth); 
			if (m_nMonth == 1)
			{ 
				m_nMonth = 13;
				--m_nYear;
			} 
			--m_nMonth; 
		}
	}
}

void Date::AddMonths(int nMonthsToAdd)
{
	m_nMonth += nMonthsToAdd;

	if (m_nMonth > 12)
	{
		AddYears(m_nMonth / 12);
		m_nMonth %= 12; // rollover dec -> jan
	}
	else if (m_nMonth < 1)
	{
		while (m_nMonth < 1)
		{
			m_nMonth += 12;
			AddYears(-1);
		}
	}
	m_nDay = min(m_nDay, GetMonthDay(m_nYear, m_nMonth));
}

void Date::AddYears(int m_nYearsToAdd)
{
	m_nYear += m_nYearsToAdd;
}

Date Date::operator +(int nDaysToAdd)
{
	Date newDate(m_nYear, m_nMonth, m_nDay);
	newDate.AddDays(nDaysToAdd);

	return newDate;
}

string Date::FormatDate(char szMask/* = '-'*/)
{
	std::stringstream ssTemp;
	ssTemp << m_nYear << szMask << setw(2) << setfill('0') << right << m_nMonth 
		<< szMask << setw(2) << setfill('0') << right << m_nDay;
	return ssTemp.str();
}

string Date::FormatYearMonth(char szMask/* = '-'*/)
{
	std::stringstream ssTemp;
	ssTemp << m_nYear << szMask << setw(2) << setfill('0') << right << m_nMonth;
	return ssTemp.str();
}

int Date::GetYear(void) const
{
	return m_nYear;
}

int Date::GetMonth(void) const
{
	return m_nMonth;
}

int Date::GetDay(void) const
{
	return m_nDay;
}

bool Date::IsLeapYear(int year) 
{ 
	if (((year % 4 == 0) && (year % 100)) || (year % 400 == 0)) 
	{ 
		return true; 
	} 
	return false; 
}

int Date::GetMonthDay(int year, int month) //用一个数组存储1-12月所占天数
{ 
	int monthArray[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; 
	int day = monthArray[month]; 
	if (month == 2 && IsLeapYear(year)) 
	{ 
		day += 1; 
	} 
	return day; 
}

}}
