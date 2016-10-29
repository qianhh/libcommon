#include "StdAfx.h"
#include "NamedPipeClient.h"
#include "Event.h"
#include "CriticalSectionLock.h"
#include "log.h"
#include "Buffer.h"
#include "common.h"
#include <process.h>

#define BUFSIZE 8192

namespace eric { namespace common {
NamedPipeClient::NamedPipeClient(void) : m_hPipe(NULL), m_hThread(NULL)
{
	memset(m_szPipeName, 0, sizeof(m_szPipeName));
}

NamedPipeClient::~NamedPipeClient(void)
{
	ClosePipe();
}

bool NamedPipeClient::Initialize(wchar_t *pszPipeName/* = L"\\\\.\\pipe\\AppService"*/)
{
	wcscpy_s(m_szPipeName, pszPipeName);
	return true;
}

void NamedPipeClient::ClosePipe(void)
{
	CLOSE_HANDLE(m_hPipe);
	if (m_hThread)
	{
		if (WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hThread, 500))
			TerminateThread((HANDLE)m_hThread, 1);
		CLOSE_HANDLE(m_hThread);
	}
}

bool NamedPipeClient::WaitPipe(unsigned int nTimeOut/* = NMPWAIT_NOWAIT*/)
{
	BOOL bRet = ::WaitNamedPipe(m_szPipeName, nTimeOut);
	if (!bRet) LOG_TRACE("Error wait pipe, error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
	return bRet==TRUE;
}

bool NamedPipeClient::OpenPipe(void)
{
	m_hPipe = (uintptr_t)CreateFileW(m_szPipeName, GENERIC_WRITE|GENERIC_READ
		, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL); 
	if ((HANDLE)m_hPipe == INVALID_HANDLE_VALUE)
	{
		m_hPipe = NULL;
		LOG_TRACE("Error open pipe, error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
		return false;
	}
	unsigned int uThreadId = 0;
	m_hThread = _beginthreadex(NULL, 0, NamedPipeClient::ThreadFunction, reinterpret_cast<void*>(this), 0, &uThreadId);
	if (!m_hThread)
	{
		CLOSE_HANDLE(m_hPipe);
		LOG_TRACE("Create thread failed, error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
		return false;
	}
	return true;
}

unsigned int WINAPI NamedPipeClient::ThreadFunction(void* pContext)
{
	NamedPipeClient *pThread = static_cast<NamedPipeClient*>(pContext);
	
	DWORD dwResult = 0;
	__try
	{
		dwResult = pThread->Execute();
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
	return dwResult;
}

unsigned int NamedPipeClient::Execute(void)
{
	DWORD dwRead = 0;
	Buffer bufRecv;
	char *pData = NULL;
	BOOL fSuccess = FALSE;
	long lSize = 0;
	while (1)
	{
		lSize = 0;
		pData = (char *)bufRecv.GetBuffer(BUFSIZE);
		do
		{
			dwRead = 0;
			fSuccess = ReadFile((HANDLE)m_hPipe, pData+lSize, BUFSIZE-lSize, &dwRead, NULL);
			lSize += dwRead;
			if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
				break; 
		} while (!fSuccess);
		if (!fSuccess)
		{
			LOG_TRACE("ReadFile from pipe failed. error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
			return 1;
		}
		if (lSize == 0) continue;
		bufRecv.ReleaseBufferSetSize(lSize);
		LOG_TRACE("Recv data of %d size", lSize);
		OnReadEvent(bufRecv);
	}
	return 0;
}

bool NamedPipeClient::Send(const Buffer &bufSend)
{
	if (!m_hPipe) return false;
	DWORD dwWrite = 0;
	bool bSuccess = false;
	do
	{
		if (!WriteFile((HANDLE)m_hPipe, bufSend.GetBuffer(), bufSend.GetSize(), &dwWrite, NULL))
		{
			LOG_TRACE("Write data error, error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
			break;
		}
		bSuccess = true;
		LOG_TRACE("Send data of %d size", bufSend.GetSize());
	} while (0);
	return bSuccess;
}

void NamedPipeClient::OnReadEvent(const Buffer &bufRecv)
{
}

}}
