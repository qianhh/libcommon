#include "stdafx.h"
#include "ProcHelper.h"
#include <Windows.h>
#include "log.h"
#include <string>
using namespace std;

namespace eric { namespace common {
bool CProcHelper::SetSigletonProc(const char *pszProcName)
{
    if (!pszProcName)
    {
        return false;
    }

    const string strMutex = pszProcName + string("_MUTEX");
    HANDLE hMutex = CreateMutexA(NULL, FALSE, strMutex.c_str());
    if (NULL == hMutex)
    {
       LOG_TRACE("CreateMutexW FAILED. GetLastError: %d", GetLastError());
        return false;
    }
    if (ERROR_ALREADY_EXISTS == GetLastError())
    {
		LOG_TRACE("Mutex %s already EXISTED.", strMutex.c_str());
        return false;
    }

    return true;
}

}}
