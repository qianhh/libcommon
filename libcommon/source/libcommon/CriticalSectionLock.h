#pragma once
#include <Windows.h>

/**
 ** 临界区封装类
 ** designed by eric,2016/08/20
 **/
class CriticalSectionAutoLock;
class CriticalSectionLock
{
	friend CriticalSectionAutoLock;
public:
	CriticalSectionLock()
	{
		::InitializeCriticalSection(&m_cs);
	}

	void Lock(void)
	{
		::EnterCriticalSection(&m_cs);
	}

	void Unlock(void)
	{
		::LeaveCriticalSection(&m_cs);
	}

	~CriticalSectionLock()
	{
		::DeleteCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;
};

/**
 ** 临界区自动锁
 ** designed by eric,2016/08/20
 **/
class CriticalSectionAutoLock
{
public:
	CriticalSectionAutoLock(CRITICAL_SECTION &cs) : m_cs(cs)
	{
		::EnterCriticalSection(&m_cs);
	}

	CriticalSectionAutoLock(CriticalSectionLock &lock) : m_cs(lock.m_cs)
	{
		::EnterCriticalSection(&m_cs);
	}

	~CriticalSectionAutoLock()
	{
		::LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION &m_cs;
};