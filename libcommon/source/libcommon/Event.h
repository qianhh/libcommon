#pragma once

/**
 ** Event·â×°Àà
 ** designed by eric,2016/08/26
 **/
namespace eric { namespace common {
class Event
{
public:
	Event(BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE, LPCTSTR pstrName = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL) : m_hObject(NULL)
	{
		m_hObject = ::CreateEvent(lpsaAttribute, bManualReset, bInitiallyOwn, pstrName);
	}

	~Event()
	{
		if (m_hObject)
		{
			::CloseHandle(m_hObject);
			m_hObject = NULL;
		}
	}

	BOOL Lock(unsigned int dwTimeout = INFINITE)
	{
		DWORD dwRes = ::WaitForSingleObject(m_hObject, dwTimeout);
		return dwRes == WAIT_OBJECT_0;
	}

	BOOL SetEvent()
	{
		return ::SetEvent(m_hObject);
	}

	BOOL PulseEvent()
	{
		return ::PulseEvent(m_hObject);
	}

	BOOL ResetEvent()
	{
		return ::ResetEvent(m_hObject);
	}

	HANDLE	m_hObject;
};

}}
