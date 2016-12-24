#pragma once

/**
 ** »’∆⁄‘ÀÀ„
 ** designed by eric,2016/11/23
 **/
namespace eric { namespace common {
class Date
{
public:
    Date(int nYear, int nMonth, int nDay);

	Date(void);

	void AddDays(int nDaysToAdd);

    void AddMonths(int nMonthsToAdd);
 
    void AddYears(int m_nYearsToAdd);
 
    Date operator +(int nDaysToAdd);
 
    std::string FormatDate(char szMask = '-');

	std::string FormatYearMonth(char szMask = '-');

	int GetYear(void) const;

	int GetMonth(void) const;

	int GetDay(void) const;

private:
	bool IsLeapYear(int year);

	int GetMonthDay(int year, int month);

private:
    int m_nDay; // Range: 1 - 30 (lets assume all months have 30 days!)
    int m_nMonth; // Range: 1 - 12
    int m_nYear;
};

}}
