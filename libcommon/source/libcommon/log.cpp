#include "stdafx.h"
#include "log.h"
#include <stdarg.h>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <process.h>
#include <io.h> 
#include "Buffer.h"
#include "Event.h"
#include "CriticalSectionLock.h"

using namespace std; 

#define PRINTF_BUF_SIZE  1024

namespace eric { namespace common {
class Date {
public:
    // Constructor that initializes the object to a day, month and year
    Date(int nYear, int nMonth, int nDay) : m_nDay(nDay), m_nMonth(nMonth), m_nYear(nYear)
	{
    }

	Date(void) : m_nDay(0), m_nMonth(0), m_nYear(0)
	{
		SYSTEMTIME systime = {0};
		::GetLocalTime(&systime);
		m_nYear = systime.wYear;
		m_nMonth = systime.wMonth;
		m_nDay = systime.wDay;
    }

	//日期加天数，因为定义的天数为int型，所以可能加一个负的天数
	//故在循环中添加了判断处理，若负则执行减操作，反之执行加操作 
	void AddDays(int nDaysToAdd)
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

    void AddMonths(int nMonthsToAdd)
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
 
    void AddYears(int m_nYearsToAdd)
	{
        m_nYear += m_nYearsToAdd;
    }
 
    Date operator +(int nDaysToAdd)
	{
        Date newDate(m_nYear, m_nMonth, m_nDay);
        newDate.AddDays(nDaysToAdd);
 
        return newDate;
    }
 
    string FormatDate(char szMask = '-')
	{
		std::stringstream ssTemp;
		ssTemp << m_nYear << szMask << setw(2) << setfill('0') << right << m_nMonth 
			<< szMask << setw(2) << setfill('0') << right << m_nDay;
		return ssTemp.str();
	}

	string FormatYearMonth(char szMask = '-')
	{
		std::stringstream ssTemp;
		ssTemp << m_nYear << szMask << setw(2) << setfill('0') << right << m_nMonth;
		return ssTemp.str();
	}

	int GetYear(void) const
	{
		return m_nYear;
	}

	int GetMonth(void) const
	{
		return m_nMonth;
	}

	int GetDay(void) const
	{
		return m_nDay;
	}

private:
	bool IsLeapYear(int year) 
	{ 
		if (((year % 4 == 0) && (year % 100)) || (year % 400 == 0)) 
		{ 
			return true; 
		} 
		return false; 
	}

	int GetMonthDay(int year, int month) //用一个数组存储1-12月所占天数
	{ 
		int monthArray[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; 
		int day = monthArray[month]; 
		if (month == 2 && IsLeapYear(year)) 
		{ 
			day += 1; 
		} 
		return day; 
	}

private:
    int m_nDay; // Range: 1 - 30 (lets assume all months have 30 days!)
    int m_nMonth; // Range: 1 - 12
    int m_nYear;
};


struct Logger::Impl
{
	Buffer m_cache;					//待写入文件的缓存
	Event m_eventWrite;				//写文件事件
	CriticalSectionLock m_lock;		//log锁
	char m_szLogPath[MAX_PATH];		//log路径
	char m_szLogDirectory[MAX_PATH];//log目录
	Logger::MOD_FILE m_modType;		//log文件模式
	bool m_reserveLog;				//是否保留log
	bool m_exit;					//退出
	HANDLE m_hWriteThread;			//写日志线程	
	unsigned int m_uWriteInterval;	//写文件时间间隔(ms)
	unsigned int m_uCacheSize;		//缓存大小
};

Logger::Logger(void) : m_pImpl(new Impl)
{
	memset(m_pImpl->m_szLogDirectory, 0, sizeof(m_pImpl->m_szLogDirectory));
	::GetModuleFileNameA(NULL, m_pImpl->m_szLogDirectory, MAX_PATH);
	strcat_s(m_pImpl->m_szLogDirectory, ".log");

	memset(m_pImpl->m_szLogPath, 0, sizeof(m_pImpl->m_szLogPath));

	m_pImpl->m_modType = ONLY_FILE;
	m_pImpl->m_reserveLog = false;
	m_pImpl->m_exit = false;
	m_pImpl->m_hWriteThread = NULL;
	m_pImpl->m_uWriteInterval = 8*1000;
	m_pImpl->m_uCacheSize = 4*1024;
}

Logger::~Logger(void)
{
	m_pImpl->m_exit = true;
	m_pImpl->m_eventWrite.SetEvent();
	if (m_pImpl->m_hWriteThread)
	{
		if (::WaitForSingleObject(m_pImpl->m_hWriteThread, 200) != WAIT_OBJECT_0)
			::TerminateThread(m_pImpl->m_hWriteThread, 1);
		CloseHandle(m_pImpl->m_hWriteThread);
	}
	DELETE_PTR(m_pImpl);
}

void Logger::SetLogDirectory(const char *path)
{
	if (path) strcpy_s(m_pImpl->m_szLogDirectory, path);
}

void Logger::SetMod(MOD_FILE type, bool reserve)
{
	m_pImpl->m_modType = type;
	m_pImpl->m_reserveLog = reserve;
}

void Logger::SetIntervalToWriteFile(unsigned int interval)
{
	m_pImpl->m_uWriteInterval = interval;
}

void Logger::SetCacheSize(unsigned int size)
{
	m_pImpl->m_uCacheSize = size;
}

void Logger::Write(const char *info, const char *fmt, ...)
{
	char szBuf[PRINTF_BUF_SIZE] = {0};	//一条log的缓存
	SYSTEMTIME systime = {0};
	::GetLocalTime(&systime);
	int nTimeCount = _snprintf_s(szBuf, PRINTF_BUF_SIZE, 32, "[%04d/%02d/%02d %02d:%02d:%02d.%03d]", systime.wYear, systime.wMonth, systime.wDay
		, systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
	strcat_s(szBuf, PRINTF_BUF_SIZE, info);
	nTimeCount += strlen(info);
	va_list args;
	va_start(args, fmt);
	int nCount = _vsnprintf_s(szBuf+nTimeCount, PRINTF_BUF_SIZE-nTimeCount, PRINTF_BUF_SIZE-nTimeCount-3, fmt, args);
	va_end(args);
	if (nCount > -1 && nCount < PRINTF_BUF_SIZE)
		nCount += nTimeCount;
	else
		nCount = PRINTF_BUF_SIZE - 3;
	szBuf[nCount] = '\r';
	szBuf[nCount+1] = '\n';
	szBuf[nCount+2] = 0;

#ifdef WRITE_FILE_FLAG
	if (!m_pImpl->m_hWriteThread)
	{
		unsigned int uThreadId = 0;
		m_pImpl->m_hWriteThread = (HANDLE)_beginthreadex(NULL, 0, Logger::ThreadFunction, reinterpret_cast<void*>(this), 0, &uThreadId);
	}

	CriticalSectionAutoLock lock(m_pImpl->m_lock);
	::OutputDebugStringA(szBuf);
	m_pImpl->m_cache.Append(szBuf, nCount+2);
	if (m_pImpl->m_cache.GetSize() > (long)m_pImpl->m_uCacheSize)
		m_pImpl->m_eventWrite.SetEvent();
#else
	CriticalSectionAutoLock lock(m_pImpl->m_lock);
	::OutputDebugStringA(pBuf);
#endif
}

void Logger::DeleteOldLogFile(void)
{
	if (m_pImpl->m_reserveLog) return;

	const long MAX_RESERVE = 6;
	string strFileName[MAX_RESERVE];
	string strCurFileName; //当天日志文件名
	Date date;
	switch (m_pImpl->m_modType)
	{
		case ONLY_FILE: //超过10m进行删除
			{
				FILE *fp = NULL;
				fopen_s(&fp, m_pImpl->m_szLogPath, "rb");
				if (fp)
				{
					fseek(fp, 0, SEEK_END);
					long lFileSize = ftell(fp);
					fclose(fp);
					if (lFileSize > 10*1024*1024)
						remove(m_pImpl->m_szLogPath);
				}
				return;
			}
			break;
		case PERDAY_FILE: //保留最近7天的日志
			{
				strCurFileName = date.FormatDate();
				date.AddDays(-1);
				for (long i = 0; i < MAX_RESERVE; ++i)
				{
					strFileName[i] = date.FormatDate();
					date.AddDays(-1);
				}
			}
			break;
		case PERMONTH_FILE: //保留最近7个月的日志
			{
				strCurFileName = date.FormatYearMonth();
				date.AddMonths(-1);
				for (long i = 0; i < MAX_RESERVE; ++i)
				{
					strFileName[i] = date.FormatYearMonth();
					date.AddMonths(-1);
				}
			}
			break;
	}
	//对保留文件进行只读保护
	string strFile;
	for (long i = 0; i < MAX_RESERVE; ++i)
	{
		strFile = m_pImpl->m_szLogDirectory;
		strFile += "\\" + strFileName[i];
		SetFileAttributesA(strFile.c_str(), FILE_ATTRIBUTE_READONLY);
	}

    WIN32_FIND_DATAA fData;
	strFile = m_pImpl->m_szLogDirectory;
	strFile += "\\*-*";
	HANDLE hFind = FindFirstFileA(strFile.c_str(), &fData);
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do
	{
        if (fData.cFileName[0] == '.' )
            continue;

        if (fData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
        {
			if (strCurFileName != fData.cFileName) //当前日志排除
				DeleteFileA(strFile.c_str());
        }
    } while(FindNextFileA(hFind, &fData));

    if (hFind) FindClose(hFind);

	//对保留文件属性进行还原
	for (long i = 0; i < MAX_RESERVE; ++i)
	{
		strFile = m_pImpl->m_szLogDirectory;
		strFile += "\\" + strFileName[i];
		SetFileAttributesA(strFile.c_str(), FILE_ATTRIBUTE_NORMAL);
	}
}

void Logger::GenerateLogFileName(void)
{
	switch (m_pImpl->m_modType)
	{
	case ONLY_FILE:
		strcpy_s(m_pImpl->m_szLogPath, m_pImpl->m_szLogDirectory);
		break;
	case PERDAY_FILE:
		{
			Date date;
			std::stringstream ssTemp;
			ssTemp << m_pImpl->m_szLogDirectory << "\\" << date.FormatDate();
			strcpy_s(m_pImpl->m_szLogPath, ssTemp.str().c_str());
		}
		break;
	case PERMONTH_FILE:
		{
			Date date;
			std::stringstream ssTemp;
			ssTemp << m_pImpl->m_szLogDirectory << "\\" << date.GetYear() << "-" << setw(2) << setfill('0') << left << date.GetMonth();
			strcpy_s(m_pImpl->m_szLogPath, ssTemp.str().c_str());
		}
		break;
	}
	if (m_pImpl->m_modType == PERDAY_FILE || m_pImpl->m_modType == PERMONTH_FILE)
	{
		WIN32_FIND_DATAA wfd = {0};
		bool bExist = false;
		HANDLE hFind = FindFirstFileA(m_pImpl->m_szLogDirectory, &wfd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				bExist = true;
			FindClose(hFind);
		}
		if (!bExist)
		{
			//创建目录
			if (!CreateDirectoryA(m_pImpl->m_szLogDirectory, NULL))
			{
				DWORD dwError = GetLastError();
				if (dwError == 183) //已存在文件 则进行删除再创建
				{
					DeleteFileA(m_pImpl->m_szLogDirectory);
					CreateDirectoryA(m_pImpl->m_szLogDirectory, NULL);
				}
			}
		}
	}
}

unsigned int Logger::Execute(void)
{
	DeleteOldLogFile(); //刚启动的时候进行日志删除操作
	while (!m_pImpl->m_exit)
	{
		m_pImpl->m_eventWrite.Lock(m_pImpl->m_uWriteInterval);

		CriticalSectionAutoLock lock(m_pImpl->m_lock);
		GenerateLogFileName();
		Buffer &cache = m_pImpl->m_cache;
		if (cache.GetSize() > 0)
		{
			FILE *fp = NULL;
			fopen_s(&fp, m_pImpl->m_szLogPath, "a");
			if (fp)
			{
				fwrite(cache.GetBuffer(), cache.GetSize(), 1, fp);
				fclose(fp);
			}
			cache.Empty();
		}

		SYSTEMTIME systime = {0};
		::GetLocalTime(&systime);
		if (systime.wHour == 5 && systime.wMinute == 0) //每天5点进行日志删除操作
			DeleteOldLogFile();
	}
	return 0;
}

unsigned int __stdcall Logger::ThreadFunction(void* pContext)
{
	Logger *pThread = static_cast<Logger*>(pContext);

	DWORD dwResult = 0;
#ifdef _DEBUG
	dwResult = pThread->Execute();
#else
	__try
	{
		dwResult = pThread->Execute();
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#endif
	return dwResult;
}

}}
