#include "stdafx.h"
#include "ProcHelper.h"
#include <Windows.h>
#include <userenv.h>
#include <WtsApi32.h>
#include <Tlhelp32.h>
#include <Psapi.h>
#include "log.h"
#include <string>
#include <map>
#include "Charset.h"
#include "OSHelper.h"
#include "PathHelper.h"
#include "FileHelper.h"
using namespace std;
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "Psapi.lib")

#define COMMON_SE_TCB_NAME     L"SeTcbPrivilege"

namespace eric { namespace common {
typedef BOOL (WINAPI *pfnLookupPrivilegeValueW)(LPCWSTR lpSystemName, LPCWSTR lpName, PLUID lpLuid);
typedef BOOL (WINAPI *pfnAdjustTokenPrivileges)(HANDLE TokenHandle, 
	BOOL DisableAllPrivileges, 
	PTOKEN_PRIVILEGES NewState,
	DWORD BufferLength,
	PTOKEN_PRIVILEGES PreviousState,
	PDWORD ReturnLength);
typedef DWORD (WINAPI *pfnWTSGetActiveConsoleSessionId)(void);
typedef BOOL (WINAPI *pfnCreateEnvironmentBlock)(LPVOID *lpEnvironment, HANDLE hToken, BOOL bInherit);
typedef BOOL (WINAPI *pfnDestroyEnvironmentBlock)(LPVOID lpEnvironment);
typedef DWORD (WINAPI *pfnWTSQueryUserToken)(ULONG SessionId, PHANDLE phToken);
typedef BOOL (WINAPI *pfnOpenProcessToken)(HANDLE ProcessHandle, DWORD DesiredAccess, PHANDLE TokenHandle);
typedef BOOL (WINAPI *pfnCreateProcessW)(LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation);
typedef BOOL (WINAPI *pfnCreateProcessAsUserW)(HANDLE hToken,
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation);
typedef BOOL (WINAPI *pfnWriteProcessMemory)(HANDLE hProcess,                   // handle to process whose memory is written to
	LPVOID lpBaseAddress,              // address to start writing to
	LPVOID lpBuffer,                   // pointer to buffer to write data to
	DWORD nSize,                       // number of bytes to write
	LPDWORD lpNumberOfBytesWritten);   // actual number of bytes written
typedef BOOL (WINAPI *pfnReadProcessMemory)(HANDLE hProcess,
	LPCVOID lpBaseAddress,
	LPVOID lpBuffer,
	DWORD nSize,
	LPDWORD lpNumberOfBytesRead);
typedef BOOL (WINAPI *pfnTerminateProcess)(HANDLE hProcess, UINT uExitCode);


///////////////////////////////////////////////////////////////////////////////
bool ProcHelper::SetSigletonProc(const wchar_t *pszProcName)
{
	if (!pszProcName)
	{
		return false;
	}

	const wstring strMutex = pszProcName + wstring(L"_MUTEX");
	HANDLE hMutex = CreateMutexW(NULL, FALSE, strMutex.c_str());
	if (NULL == hMutex)
	{
		LOG_TRACE("CreateMutexW FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
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

bool ProcHelper::CreateProcessInherit(const wstring& cmd, 
	const wstring& appName,
	const wstring& workingDir,
	const bool bShowWindow,
	const bool bInherit,
	const bool bWait,
	DWORD dwWaitTime,
	DWORD* pThreadID,
	DWORD* pProcessID,
	DWORD* pExitCode)
{
	PROCESS_INFORMATION pi = {0};

	STARTUPINFOW si = {0};
	si.cb = sizeof(STARTUPINFOW);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = bShowWindow ? SW_SHOW : SW_HIDE;

	wchar_t* szCmd = new wchar_t[cmd.length() + 1];
	if (szCmd == NULL)
	{
		LOG_TRACE("szCmd new FAILED.");
		return false;
	}
	memset(szCmd, 0, cmd.length() + 1);
	wsprintfW(szCmd, L"%s", cmd.c_str());

	bool bRt = true;
	do 
	{
		if (!CreateProcessW(appName.length() <= 0 ? NULL : appName.c_str(),
			szCmd,
			NULL,
			NULL,
			bInherit ? TRUE : FALSE,
			NORMAL_PRIORITY_CLASS,
			0,
			workingDir.length() <= 0 ? NULL : workingDir.c_str(),
			&si,
			&pi))
		{
			Buffer buf;
			Charset::UnicodeToGB2312(cmd.c_str(), buf);
			string strCmd((char *)buf.GetBuffer(), buf.GetSize());
			Charset::UnicodeToGB2312(appName.c_str(), buf);
			string strAppName((char *)buf.GetBuffer(), buf.GetSize());
			LOG_TRACE("CreateProcessW FAILED. GetLastError[%d]: %s; cmd: %s; appName: %s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str(), strCmd.c_str(), strAppName.c_str());
			bRt = false;
			break;
		}


		if (bWait && NULL != pi.hProcess)
		{
			DWORD dwSingleRet = ::WaitForSingleObject(pi.hProcess, dwWaitTime);
			if (WAIT_OBJECT_0 != dwSingleRet)
			{
				LOG_TRACE("::WaitForSingleObject FAILED. GetLastError[%d]: %s; dwSingleRet: %d", GetLastError(), GetLastErrorMessage(GetLastError()).c_str(), dwSingleRet);
				bRt = false;
				break;
			}
		}

		if (NULL != pExitCode && NULL != pi.hProcess && bRt)
		{
			if (!GetExitCodeProcess(pi.hProcess, pExitCode))
			{
				LOG_TRACE("GetExitCodeProcess FAILED. GetLastError[%d]: %s; pExitCode: %d", GetLastError(), GetLastErrorMessage(GetLastError()).c_str(), *pExitCode);
				bRt = false;
				break;
			}
		}

		if (NULL != pThreadID)
		{
			*pThreadID = pi.dwThreadId;
		}

		if (NULL != pProcessID)
		{
			*pProcessID = pi.dwProcessId;
		}
	} while (false);

	CLOSE_HANDLE(pi.hProcess);
	CLOSE_HANDLE(pi.hThread);
	DELETE_PTR(szCmd);

	return bRt;
}

bool ProcHelper::MyCreateProcessAsUser(const wstring& path, const wstring& cmd/* = L""*/, const bool bShowWnd/* = false*/)
{
	HANDLE hToken = NULL;
	MySetPrivilege(COMMON_SE_TCB_NAME, true);

	DWORD dwSessId = WTSGetActiveConsoleSessionId();
	if (!WTSQueryUserToken(dwSessId, &hToken))
	{
		LOG_TRACE("WTSQueryUserToken FAILED. GetLastError[%d]: %s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
		return false;
	}

	PROCESS_INFORMATION pi = {0};

	STARTUPINFOW si = {0};
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = L"winsta0\\default";
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = bShowWnd ? 1 : 0;

	wchar_t* szCmd = new wchar_t[cmd.length() + 1];
	if (szCmd == NULL)
	{
		LOG_TRACE("szCmd new FAILED.");
		return false;
	}
	memset(szCmd, 0, cmd.length() + 1);
	wsprintfW(szCmd, L"%s", cmd.c_str());

	// 不传递Env会导致启动的程序取不到用户文件夹，如：SHGetFolderPath失败
	LPVOID pEnv = NULL;
	CreateEnvironmentBlock(&pEnv, hToken, FALSE);

	bool bRet = true;
	if (false == CreateProcessAsUserW(hToken, 
		path.c_str(), 
		szCmd, 
		NULL, 
		NULL, 
		FALSE,
		NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT,
		pEnv, 
		NULL, 
		&si, 
		&pi))
	{
		LOG_TRACE("CreateProcessAsUserW FAILED. GetLastError[%d]: %s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
		bRet = false;
	}

	DestroyEnvironmentBlock(pEnv);

	CLOSE_HANDLE(pi.hProcess);
	CLOSE_HANDLE(pi.hThread);
	CLOSE_HANDLE(hToken);
	DELETE_PTR(szCmd);

	return bRet;
}

wstring ProcHelper::GetCurrentProcessPathName(void)
{
	wstring path;

	wchar_t szPath[MAX_PATH] = {0};
	if (0 != GetModuleFileNameW(NULL, szPath, MAX_PATH))
	{
		path = szPath;
	}

	return path;
}

wstring ProcHelper::GetCurrentProcessPath(void)
{
	wstring path = GetCurrentProcessPathName();

	if (!path.empty())
	{
		try
		{
			path = path.erase(path.rfind(L"\\"));
		}
		catch (exception e)
		{
			path = L"";
			LOG_TRACE("string erase catch EXCEPTION, error: %s", e.what());
		}
	}

	return path;
}

wstring ProcHelper::GetCurrentProcessName(void)
{
	wstring name = GetCurrentProcessPathName();
	if (!name.empty())
	{
		try
		{
			size_t pos = name.rfind(L"\\");
			if (wstring::npos != pos)
			{
				name = name.substr(pos + 1, name.length() - pos);
			}
		}
		catch (exception e)
		{
			name = L"";
			LOG_TRACE("string substr catch EXCEPTION, error: %s", e.what());
		}
	}

	return name;
}

HMODULE ProcHelper::GetCurrentModule(void)
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)GetCurrentModule, &hModule);
	return hModule;
}

wstring ProcHelper::GetCurrentModulePathName(void)
{
	wstring path;

	wchar_t szPath[MAX_PATH] = {0};
	if (0 != GetModuleFileNameW(GetCurrentModule(), szPath, MAX_PATH))
	{
		path = szPath;
	}

	return path;
}

wstring ProcHelper::GetCurrentModulePath(void)
{
	wstring path = GetCurrentModulePathName();
	if (!path.empty())
	{
		try
		{
			path = path.erase(path.rfind(L"\\"));
		}
		catch (exception e)
		{
			path = L"";
			LOG_TRACE("string erase catch EXCEPTION, error: %s", e.what());
		}
	}

	return path;
}

wstring ProcHelper::GetCurrentModuleName(void)
{
	wstring name = GetCurrentModulePathName();
	if (!name.empty())
	{
		try
		{
			size_t pos = name.rfind(L"\\");
			if (wstring::npos != pos)
			{
				name = name.substr(pos + 1, name.length() - pos);
			}
		}
		catch (exception e)
		{
			name = L"";
			LOG_TRACE("string substr catch EXCEPTION, error: %s", e.what());
		}
	}

	return name;
}

bool ProcHelper::RegisterDLL(const wstring& dllPath, bool bSilent/* = true*/)
{
	wstring regCmd = L"regsvr32.exe ";
	if (bSilent)
	{
		regCmd = L"regsvr32.exe /s ";
	}

	regCmd = regCmd + L"\"" + dllPath + L"\"";

	return CreateProcessInherit(regCmd, L"", L"", false, true, true, 1000 * 2);
}

bool ProcHelper::UnregisterDLL(const wstring& dllPath, bool bSilent/* = true*/)
{
	wstring regCmd = L"regsvr32.exe /u ";
	if (bSilent)
	{
		regCmd = L"regsvr32.exe /s /u ";
	}

	regCmd = regCmd + L"\"" + dllPath + L"\"";

	return CreateProcessInherit(regCmd, L"", L"", false, true, true, 1000 * 2);
}

HMODULE ProcHelper::GetModule(const wstring& modulePath)
{
	if (modulePath.empty())
	{
		return NULL;
	}

	HMODULE hModule = GetModuleHandleW(modulePath.c_str());
	if (NULL == hModule)
	{
		hModule = LoadLibraryW(modulePath.c_str());
	}

	if (NULL == hModule)
	{
		Buffer buf;
		Charset::UnicodeToGB2312(modulePath.c_str(), buf);
		LOG_TRACE("CProcHelper::GetModule FAILED. szModule: %s. GetLastError[%d]: %s", (char *)buf.GetBuffer(), GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
		return NULL;
	}

	return hModule;
}

bool ProcHelper::LaunchProcess(const wstring& filePath)
{
	STARTUPINFOW si; 
	PROCESS_INFORMATION pi; 
	ZeroMemory(&si, sizeof(STARTUPINFOW)); 
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION)); 
	si.cb = sizeof(STARTUPINFO); 
	si.lpDesktop = L"WinSta0\\Default"; 	
	DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT; 
	HMODULE hKernel32 = GetModule(L"Kernel32.dll");
	if(hKernel32 == NULL)
	{
		return false;
	}

	pfnCreateProcessW create_process = (pfnCreateProcessW)GetProcAddress(hKernel32, "CreateProcessW");
	if(create_process == NULL)
	{
		FREE_LIBRARY(hKernel32);
		return false;
	}

	bool bRet = create_process(NULL, (wchar_t*)filePath.c_str(), NULL, NULL, FALSE, dwCreationFlag, NULL, NULL, &si, &pi) ? true : false;
	if(!bRet)
	{
		LOG_TRACE("create_process FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage().c_str());
	}

	FREE_LIBRARY(hKernel32);
	return bRet;
}

bool ProcHelper::LaunchProcessAsUser(const wstring& strFilePath, const wstring& strCmdLine/* = L""*/, bool bShowWnd/* = false*/)
{
	if (strFilePath.empty())
	{
		return false;
	}

	bool bRet = false;
	HMODULE hKernel32 = NULL;
	HMODULE hWtsapi32 = NULL;
	HMODULE hUserenv = NULL;
	HMODULE hAdvAPI = NULL;
	DWORD dwSessionId = 0;

	pfnWTSQueryUserToken  WTSQueryUserToken = NULL;
	pfnWTSGetActiveConsoleSessionId  WTSGetActiveConsoleSessionId = NULL;
	pfnCreateEnvironmentBlock  CreateEnvironmentBlock = NULL;
	pfnDestroyEnvironmentBlock DestroyEnvironmentBlock = NULL;
	pfnCreateProcessAsUserW	   create_process_as_user = NULL;
	pfnOpenProcessToken			pOpenProcessToken = NULL;

	std::wstring dir;
	std::wstring filename;

	STARTUPINFOW si; 
	PROCESS_INFORMATION pi; 
	ZeroMemory(&si, sizeof(STARTUPINFOW)); 
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION)); 
	si.cb = sizeof(STARTUPINFOW); 
	si.lpDesktop = L"WinSta0\\Default";
	si.wShowWindow = bShowWnd ? 1 : 0;

	LPVOID pEnv = NULL; 
	DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT; 
	HANDLE hCurSessionToken = NULL;

	hKernel32 = GetModule(L"Kernel32.dll");
	if(hKernel32 == NULL)
	{
		goto clean;
	}

	hWtsapi32 = GetModule(L"Wtsapi32.dll");
	if(hWtsapi32 == NULL)
	{
		goto clean;
	}

	hUserenv = GetModule(L"Userenv.dll");
	if(hUserenv == NULL)
	{
		goto clean;
	}

	hAdvAPI = GetModule(L"advapi32.dll");
	if(hAdvAPI == NULL)
	{
		goto clean;
	}

	//SE_TCB_NAME
	MySetPrivilege(COMMON_SE_TCB_NAME, true);

	WTSGetActiveConsoleSessionId = (pfnWTSGetActiveConsoleSessionId)GetProcAddress(hKernel32,"WTSGetActiveConsoleSessionId");
	if(WTSGetActiveConsoleSessionId == NULL)
	{
		goto clean;
	}
	dwSessionId = WTSGetActiveConsoleSessionId(); 			

	WTSQueryUserToken = (pfnWTSQueryUserToken)GetProcAddress(hWtsapi32,"WTSQueryUserToken");
	if(WTSQueryUserToken == NULL)
	{
		LOG_TRACE("GetProcAddress <WTSQueryUserToken> FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage().c_str());
		goto clean;
	}

	//获取当前Session 登录ID的 Token
	bRet = 0 == WTSQueryUserToken(dwSessionId,&hCurSessionToken);
	if(!bRet)
	{
		LOG_TRACE("WTSQueryUserToken FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage().c_str());
		goto clean;
	}

	CreateEnvironmentBlock = (pfnCreateEnvironmentBlock)GetProcAddress(hUserenv,"CreateEnvironmentBlock");
	if (CreateEnvironmentBlock == NULL)
	{
		LOG_TRACE("GetProcAddress <CreateEnvironmentBlock> FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage().c_str());
		goto clean;
	}

	if (!CreateEnvironmentBlock(&pEnv,hCurSessionToken,FALSE))
	{
		LOG_TRACE("CreateEnvironmentBlock FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage().c_str());
		goto clean;
	}

	DestroyEnvironmentBlock = (pfnDestroyEnvironmentBlock)GetProcAddress(hUserenv,"DestroyEnvironmentBlock");
	if(DestroyEnvironmentBlock == NULL)
	{
		LOG_TRACE("GetProcAddress <DestroyEnvironmentBlock> FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage().c_str());
		goto clean;
	}

	if(!FileHelper::SplitDirectoryAndFileName(strFilePath.empty() ? strCmdLine : strFilePath, dir, filename))
	{
		LOG_TRACE("SplitDirectoryAndFileName FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage().c_str());
		goto clean;
	}

	create_process_as_user = (pfnCreateProcessAsUserW)GetProcAddress(hAdvAPI,"CreateProcessAsUserW");
	if(create_process_as_user == NULL)
	{
		LOG_TRACE("GetProcAddress <CreateProcessAsUserW> FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage().c_str());
		goto clean;
	}

	wchar_t* szCmd = new wchar_t[strCmdLine.length() + 1];
	if (szCmd == NULL)
	{
		LOG_TRACE("szCmd new FAILED.");
		goto clean;
	}
	memset(szCmd, 0, strCmdLine.length() + 1);
	wsprintfW(szCmd, L"%s", strCmdLine.c_str());

	bRet = create_process_as_user(hCurSessionToken, strFilePath.c_str(), szCmd, NULL, NULL, FALSE, dwCreationFlag, pEnv, dir.c_str(), &si, &pi) ? true : false; 
	if (!bRet)
	{
		LOG_TRACE("create_process_as_user FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage().c_str());
		goto clean;
	}

clean:
	FREE_LIBRARY(hKernel32);
	FREE_LIBRARY(hUserenv);
	FREE_LIBRARY(hWtsapi32);
	CLOSE_HANDLE(hCurSessionToken);
	DELETE_PTR(szCmd);

	return bRet;
}

bool ProcHelper::IsProcessRunAsAdmin()
{
	if(OSHelper::WinXP == OSHelper::GetWindowsVersion())
	{
		return true;
	}

    PSID psid;
	SID_IDENTIFIER_AUTHORITY ntAutority = SECURITY_NT_AUTHORITY;
	BOOL bTmp = AllocateAndInitializeSid(&ntAutority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,	0, 0, 0, 0, 0, 0, &psid);
	if (bTmp)
	{
		CheckTokenMembership(NULL, psid, &bTmp);
		FreeSid(psid);
	}

	return TRUE == bTmp;
}

bool ProcHelper::IsSystemProcess(const DWORD& procID/* = 0*/)
{
    DWORD dwTmpID = procID;
    if (0 >= dwTmpID)
    {
        dwTmpID = GetCurrentProcessId();
    }

    wstring userName = GetProcessUserName(dwTmpID);
    if (0 == _wcsicmp(userName.c_str(), L"system"))
    {
        return true;
    }

    return false;
}

bool ProcHelper::IsSystemProcess(const wstring& procName/* = L""*/)
{
	DWORD procID = GetProcessIDByName(procName);

	return IsSystemProcess(procID);
}

bool ProcHelper::IsProcessExist(const DWORD& procID)
{
	wstring procName = GetProcessNameByPid(procID);
	return IsProcessExist(procName);
}

bool ProcHelper::IsProcessExist(const wstring& strProcName)
{
	if (strProcName.empty())
	{
		return false;
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return false;
	}

	PROCESSENTRY32W pe = {0};
	pe.dwSize = sizeof(PROCESSENTRY32W);
	if (!Process32FirstW(hSnapshot, &pe))
	{
		LOG_TRACE("Process32FirstW FAILED. GetLastError[%d]:%s", GetLastError(), GetLastErrorMessage().c_str());
		CLOSE_HANDLE(hSnapshot);
		return false;
	}

	while (Process32NextW(hSnapshot, &pe))
	{
		if (0 == _wcsicmp(pe.szExeFile, strProcName.c_str()))
		{		
			return true;
		}
	}

	CLOSE_HANDLE(hSnapshot);
	return false;
}

bool ProcHelper::GetParentProcessName(wstring& parentProcName, DWORD* procID/* = NULL*/)
{
    HANDLE hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(PROCESSENTRY32);
    BOOL bRet = ::Process32First(hSnapShot, &pe);
    if (!bRet)
    {
		CLOSE_HANDLE(hSnapShot);
        return false;
    }

    map<DWORD, DWORD> mapParentProcIDs;
    map<DWORD, wstring> mapParentProcNames;
    while (bRet)
    {
        mapParentProcIDs.insert(map<DWORD, DWORD>::value_type(pe.th32ProcessID, pe.th32ParentProcessID));
        mapParentProcNames.insert(map<DWORD, wstring>::value_type(pe.th32ProcessID, pe.szExeFile));
        ZeroMemory(&pe, sizeof(PROCESSENTRY32));
        pe.dwSize = sizeof(PROCESSENTRY32);
        bRet = Process32Next(hSnapShot, &pe);
    }
    ::CloseHandle(hSnapShot);

    map<DWORD, DWORD>::const_iterator citer = mapParentProcIDs.find(::GetCurrentProcessId());
    if (citer == mapParentProcIDs.end())
    {
        return false;
    }

    if(mapParentProcNames.count(citer->second) == 0)
    {
        return false;
    }

    parentProcName = mapParentProcNames.find(citer->second)->second;
    if (NULL != procID)
    {
        *procID = citer->second;
    }

    return true;
}

wstring ProcHelper::GetProcessUserName(const DWORD& procID/* = 0*/)
{
    DWORD dwTmpID = procID;
    if (0 == dwTmpID || 0 > dwTmpID)
    {
        dwTmpID = GetCurrentProcessId();
    }

    wchar_t szUserName[MAX_PATH] = {0};
    wchar_t szDomain[MAX_PATH]= {0};
    DWORD dwDomainSize = MAX_PATH;
    DWORD dwNameSize = MAX_PATH;
    HANDLE	hToken	= NULL;
    BOOL	bResult	= FALSE;
    DWORD	dwSize	= 0;
    SID_NAME_USE SNU;
    PTOKEN_USER pTokenUser=NULL;

    MySetPrivilege(SE_DEBUG_NAME, true);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,dwTmpID);
    if( hProcess == NULL)
    {
        goto clean;
    }	  
    if(!OpenProcessToken(hProcess,TOKEN_QUERY,&hToken) )
    {
        goto clean;
    }
    if(!GetTokenInformation(hToken,TokenUser,pTokenUser,dwSize,&dwSize) )
    {
        if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
        {
            goto clean;
        }
    }

    pTokenUser = NULL;
    pTokenUser = (PTOKEN_USER)malloc(dwSize);
    if( pTokenUser == NULL )
    {
        goto clean;
    }
    if(!GetTokenInformation(hToken,TokenUser,pTokenUser,dwSize,&dwSize) )
    {
        goto clean;;
    }
    if(!LookupAccountSidW(NULL,pTokenUser->User.Sid,szUserName,&dwNameSize,szDomain,&dwDomainSize,&SNU) != 0 )
    {
        goto clean;
    }
clean:
    if( pTokenUser != NULL)
    {
        free(pTokenUser);
        pTokenUser = NULL;
    }
    return wstring(szUserName);
}

bool ProcHelper::MySetPrivilege(const wstring& privilege, const bool bEnablePrivilege)
{
	bool bRet = true;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	memset(&tp, 0, sizeof(TOKEN_PRIVILEGES));
	memset(&tpPrevious, 0, sizeof(TOKEN_PRIVILEGES));
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

	HANDLE hTokenThis = NULL; 
	HANDLE hThisProcess = GetCurrentProcess(); 

	pfnOpenProcessToken pOpenProcessToken = NULL;
	pfnLookupPrivilegeValueW	pLookupPrivilegeValueW = NULL;
	pfnAdjustTokenPrivileges	pAdjustTokenPrivileges = NULL;

	HMODULE hAdvAPI = GetModule(L"advapi32.dll");
	if(hAdvAPI == NULL)
	{
        LOG_TRACE("GetModule::advapi32.dll FAILED. GetLastError: %d", GetLastError());
		return false;
	}

	pOpenProcessToken = (pfnOpenProcessToken)GetProcAddress(hAdvAPI,"OpenProcessToken");
	if(pOpenProcessToken == NULL)
	{
        LOG_TRACE("GetProcAddress::OpenProcessToken FAILED. GetLastError: %d", GetLastError());
        bRet = false;
        goto clean;
	}

	pLookupPrivilegeValueW = (pfnLookupPrivilegeValueW)GetProcAddress(hAdvAPI,"LookupPrivilegeValueW");
	if(pLookupPrivilegeValueW == NULL)
	{
        LOG_TRACE("GetProcAddress::LookupPrivilegeValueW FAILED. GetLastError: %d", GetLastError());
        bRet = false;
        goto clean;
	}

	pAdjustTokenPrivileges = (pfnAdjustTokenPrivileges)GetProcAddress(hAdvAPI,"AdjustTokenPrivileges");
	if(pAdjustTokenPrivileges == NULL)
	{
		LOG_TRACE("GetProcAddress::AdjustTokenPrivileges FAILED. GetLastError: %d", GetLastError());
        bRet = false;
        goto clean;
	}

	bRet = pOpenProcessToken(hThisProcess,TOKEN_ADJUST_PRIVILEGES,&hTokenThis) ? true : false;
	if (!bRet)
	{
        LOG_TRACE("pOpenProcessToken FAILED. GetLastError:  %d", GetLastError());
        bRet = false;
        goto clean;
	}

	bRet = pLookupPrivilegeValueW(NULL, privilege.c_str(), &luid) ? true : false;
	if (!bRet)
	{
        LOG_TRACE("LookupPrivilegeValue FAILED. GetLastError:  %d", GetLastError());
        bRet = false;
        goto clean;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
    {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else
    {
        tp.Privileges[0].Attributes = 0;
    }

    bRet = pAdjustTokenPrivileges(hTokenThis,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),(PTOKEN_PRIVILEGES)NULL, (DWORD)NULL) ? true : false;
	if (!bRet)  
	{
        LOG_TRACE("AdjustTokenPrivileges FAILED. GetLastError: %d", GetLastError());
        bRet = false;
        goto clean;
	}

	//*/
    tpPrevious.PrivilegeCount = 1;
	tpPrevious.Privileges[0].Luid = luid;
	if(bEnablePrivilege)
	{
		tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	}
	else 
	{
		tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
			tpPrevious.Privileges[0].Attributes);
	}

	bRet = pAdjustTokenPrivileges(hTokenThis,FALSE,&tpPrevious,cbPrevious,NULL,NULL) ? true : false;
	if (!bRet) 
	{
        LOG_TRACE("AdjustTokenPrivileges FAILED. GetLastError: %d", GetLastError());
        bRet = false;
        goto clean;
	}
    //*/

clean:
    CLOSE_HANDLE(hTokenThis);
    CLOSE_HANDLE(hThisProcess);
    FREE_LIBRARY(hAdvAPI);

	return bRet;
}

wstring ProcHelper::GetProcessNameByPid(const DWORD dwProcID)
{
	wstring strRet;

	HANDLE hProcSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (INVALID_HANDLE_VALUE == hProcSnap) 
	{ 
        LOG_TRACE("CreateToolhelp32Snapshot FAILED. GetLastError: %d", GetLastError());
		return strRet; 
	}
    
	PROCESSENTRY32W pe32;;
	pe32.dwSize = sizeof(pe32);
    if (!Process32FirstW(hProcSnap, &pe32))
    {
        CLOSE_HANDLE(hProcSnap);
        LOG_TRACE("Process32FirstW FAILED. The specified process is a 64-bit process and the caller is a 32-bit process. GetLastError: %d", GetLastError());
        return strRet; 
    }

    wchar_t szProcName[MAX_PATH] = {0};
	while (Process32NextW(hProcSnap, &pe32)) 
	{ 
		if (dwProcID == pe32.th32ProcessID)
		{ 
			wcscpy_s(szProcName, pe32.szExeFile);
			break;
		}
	} 
    strRet = szProcName;

    CLOSE_HANDLE(hProcSnap);

	return strRet;
}

DWORD ProcHelper::GetProcessIDByName(const wstring& procName, const wstring& strFlagStr/* = L""*/)
{
    DWORD dwRet = 0;
    if (procName.empty())
    {
        return dwRet;
    }

    HANDLE hProcSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (INVALID_HANDLE_VALUE == hProcSnap) 
    { 
        LOG_TRACE("CreateToolhelp32Snapshot FAILED. GetLastError: %d", GetLastError());
        return dwRet; 
    }

    PROCESSENTRY32W pe32;;
    pe32.dwSize = sizeof(pe32);
    if (!Process32FirstW(hProcSnap, &pe32))
    {
        CLOSE_HANDLE(hProcSnap);
        LOG_TRACE("Process32FirstW FAILED. The specified process is a 64-bit process and the caller is a 32-bit process. GetLastError: %d", GetLastError());
        return dwRet; 
    }

    while (Process32NextW(hProcSnap, &pe32)) 
    { 
        if (0 != _wcsicmp(procName.c_str(), pe32.szExeFile))
        { 
            continue;
        }

        if (strFlagStr.empty())
        {
            dwRet = pe32.th32ProcessID;
            break;
        }
        else
        {
            HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
            if (NULL == hProc)
            {
                continue;
            }

            wchar_t szPath[MAX_PATH] = {0};
            if (0 == GetModuleFileNameExW(hProc, NULL, szPath, MAX_PATH))
            {
                continue;
            }

            if (wstring::npos == wstring(szPath).find(strFlagStr))
            {
                continue;
            }

            dwRet = pe32.th32ProcessID;
            CLOSE_HANDLE(hProc);
        }
    } 

    CLOSE_HANDLE(hProcSnap);

    return dwRet;
}

}}
