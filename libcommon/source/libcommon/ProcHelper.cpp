#include "stdafx.h"
#include "ProcHelper.h"
#include <Windows.h>
#include "log.h"
#include <string>
#include "Charset.h"
using namespace std;

namespace eric { namespace common {
bool CProcHelper::SetSigletonProc(const wchar_t *pszProcName)
{
    if (!pszProcName)
    {
        return false;
    }

    const wstring strMutex = pszProcName + wstring(L"_MUTEX");
    HANDLE hMutex = CreateMutexW(NULL, FALSE, strMutex.c_str());
    if (NULL == hMutex)
    {
       LOG_TRACE("CreateMutexW FAILED. GetLastError: %d", GetLastError());
        return false;
    }
    if (ERROR_ALREADY_EXISTS == GetLastError())
    {
		Buffer buf;
		Charset::UnicodeToGB2312(strMutex.c_str(), buf);
		LOG_TRACE("Mutex %s already EXISTED.", buf.GetBuffer());
        return false;
    }

    return true;
}

}}
