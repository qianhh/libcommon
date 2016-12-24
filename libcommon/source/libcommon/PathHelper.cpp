#include "stdafx.h"
#include "PathHelper.h"
#include "FileHelper.h"
#include "StringHelper.h"
#include "ProcHelper.h"
#include "OSHelper.h"
#include "RegHelper.h"
#include "Charset.h"
#include "common.h"
#include "log.h"

using namespace std;

#define C_USER_WIN7     L"C:\\Users\\"
#define C_USER_XP       L"C:\\Documents and Settings\\"

namespace eric { namespace common {
PathHelper::PathHelper() : m_strLocal(L""),
							 m_strLocalLow(L""), 
							 m_strRoaming(L""),    
							 m_strAllUser(L""),
							 m_strDesktop(L""),
							 m_strStartmenu(L""),
							 m_strTemprary(L""),   
							 m_strProgramFiles(L""),
							 m_strInstall(L""),
							 m_strConfig(L"")
{
	m_isSysProc = ProcHelper::IsSystemProcess(0);
	m_strUserName = ProcHelper::GetProcessUserName(ProcHelper::GetProcessIDByName(L"explorer.exe"));
}

PathHelper::~PathHelper()
{
}

wstring PathHelper::GetLocalPath()
{	
	if (FileHelper::IsFileOrDirExist(m_strLocal))
	{
		return m_strLocal;
	}

	// 如果是系统进程,使用当前登录的用户名来寻找路径
	if (m_isSysProc && !m_strUserName.empty())
	{		
		if(OSHelper::IsVistaOrHigher())
		{
			m_strLocal.append(C_USER_WIN7).append(m_strUserName).append(L"\\AppData\\Local");
		}
		else
		{
			m_strLocal.append(C_USER_XP).append(m_strUserName).append(L"\\Local Settings\\Application Data");
		}
	}
	else
	{
		m_strLocal = GetPathFromReg(HKEY_CURRENT_USER, REG_ITEM_SHELL_FOLDER, L"Local AppData");       
		if (m_strLocal.length() <= 0)
		{
			m_strLocal = GetKnownFolderPath(FOLDERID_LocalAppData);
		}
	}

	return m_strLocal;
}

wstring PathHelper::GetLocalLowPath()
{
	if (FileHelper::IsFileOrDirExist(m_strLocalLow))
	{
		return m_strLocalLow;
	}
	// 如果是系统进程,使用当前登录的用户名来寻找路径
	if (m_isSysProc && !m_strUserName.empty())
	{		
		if(OSHelper::IsVistaOrHigher())
		{
			m_strLocalLow.append(C_USER_WIN7).append(m_strUserName).append(L"\\AppData\\LocalLow");
		}
	}
	else
	{
		m_strLocalLow = GetPathFromReg(HKEY_CURRENT_USER, REG_ITEM_SHELL_FOLDER, L"{A520A1A4-1780-4FF6-BD18-167343C5AF16}");
		if (m_strLocalLow.length() <= 0)
		{
			m_strLocalLow = GetKnownFolderPath(FOLDERID_LocalAppDataLow);
		}
	}

	return m_strLocalLow;
}

wstring PathHelper::GetRoamingPath()
{
	if (FileHelper::IsFileOrDirExist(m_strRoaming))
	{
		return m_strRoaming;
	}

	// 如果是系统进程,使用当前登录的用户名来寻找路径
	if (m_isSysProc && !m_strUserName.empty())
	{		
		if(OSHelper::IsVistaOrHigher())
		{
			m_strRoaming.append(C_USER_WIN7).append(m_strUserName).append(L"\\AppData\\Roaming");
		}
		else
		{
			m_strRoaming.append(C_USER_XP).append(m_strUserName).append(L"\\Application Data");
		}
	}
	else
	{
		m_strRoaming = GetPathFromReg(HKEY_CURRENT_USER, REG_ITEM_SHELL_FOLDER, L"AppData");        
		if (m_strRoaming.empty())
		{
			m_strRoaming = GetKnownFolderPath(FOLDERID_RoamingAppData);
		}
	}

	return m_strRoaming;
}

wstring PathHelper::GetAllUserAppDataPath()
{
	if (FileHelper::IsFileOrDirExist(m_strAllUser))
	{
		return m_strAllUser;
	}

	m_strAllUser = GetPathFromEvr(L"ALLUSERSPROFILE", L"");

	return m_strAllUser;
}

wstring PathHelper::GetDesktopPath()
{
	if (FileHelper::IsFileOrDirExist(m_strDesktop))
	{
		return m_strDesktop;
	}

	// 如果是系统进程,使用当前登录的用户名来寻找路径
	if (m_isSysProc && !m_strUserName.empty())
	{		
		if(OSHelper::IsVistaOrHigher())
		{
			m_strDesktop.append(C_USER_WIN7).append(m_strUserName).append(L"\\Desktop");
		}
		else
		{
			m_strDesktop.append(C_USER_XP).append(m_strUserName).append(L"\\Desktop");
		}
	}
	else
	{
		m_strDesktop = GetPathFromReg(HKEY_CURRENT_USER, REG_ITEM_SHELL_FOLDER, L"Desktop");       
		if (m_strDesktop.empty())
		{
			m_strDesktop = GetKnownFolderPath(FOLDERID_Desktop);
		}
	}

	return m_strDesktop;
}

wstring PathHelper::GetStartmenuPath()
{
	if (FileHelper::IsFileOrDirExist(m_strStartmenu))
	{
		return m_strStartmenu;
	}

	// 如果是系统进程,使用当前登录的用户名来寻找路径
	if (m_isSysProc && !m_strUserName.empty())
	{		
		if(OSHelper::IsVistaOrHigher())
		{
			m_strStartmenu.append(C_USER_WIN7).append(m_strUserName).append(L"\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs");
		}
		else
		{
			// XP下不支持，因为中文系统文件夹为：\\「开始」菜单\\程序
			//m_strStartmenu.append(C_USER_XP).append(m_strUserName).append(L"\\Start Menu\\Programs");
		}
	}
	else
	{
		m_strStartmenu = GetPathFromReg(HKEY_CURRENT_USER, REG_ITEM_SHELL_FOLDER, L"Programs");
		if (m_strStartmenu.empty())
		{
			m_strStartmenu = GetKnownFolderPath(FOLDERID_StartMenu);
		}
	}

	return m_strStartmenu;
}

wstring PathHelper::GetProgramFilesPath()
{
	wstring path = GetPathFromReg(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion", L"ProgramFilesDir");

	return path.empty() ? GetKnownFolderPath(FOLDERID_ProgramFiles) : path;
}

wstring PathHelper::GetProgramFilesX86Path()
{
	wstring path;
	if(OSHelper::IsWow64())
	{
		path = GetPathFromReg(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion", L"ProgramFilesDir (x86)");
		if (path.empty())
		{
			path = GetKnownFolderPath(FOLDERID_ProgramFilesX86);
		}
	}
	else
	{
		path = GetProgramFilesPath();
	}	
	return path;
}

wstring PathHelper::GetSystemDriver()
{
	wchar_t sd[MAX_PATH] = {0};
	::GetSystemDirectoryW(sd, MAX_PATH);
	wstring system = wstring(sd, 1);
	system.append(L":");
	return system;
}


// Get path from register item: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders.
wstring PathHelper::GetPathFromReg(const HKEY& hKey, const wstring& subKey, const wstring& name, bool boSamDesired)
{
	RegHelper reg;
	reg.SetPrimaryKey(hKey);
	wstring path;
	DWORD dwType = 0;
	if (boSamDesired)
	{
		//x86程序默认读取KEY_WOW64_32KEY, x64程序默认读取KEY_WOW64_64KEY
		DWORD dwSamDesired = KEY_WOW64_32KEY;
		if (reg.ReadRegValue(subKey, name, path, dwType, dwSamDesired))
		{
			return path;
		}
		else
		{
			dwSamDesired = KEY_WOW64_64KEY;
			reg.ReadRegValue(subKey, name, path, dwType, dwSamDesired);
			return path;
		}
	}
	else
		return reg.ReadRegValue(subKey, name, path, dwType) ? path : L"";
}

wstring PathHelper::GetTemporaryPath()
{
	if (FileHelper::IsFileOrDirExist(m_strTemprary))
	{
		return m_strTemprary;
	}

		if (m_isSysProc && !m_strUserName.empty())
		{		
			if(OSHelper::IsVistaOrHigher())
			{
				m_strTemprary.append(C_USER_WIN7).append(m_strUserName).append(L"\\AppData\\Local\\Temp");
			}
			else
			{
				m_strTemprary.append(C_USER_XP).append(m_strUserName).append(L"\\Local Settings\\Temp");
			}
		}
		else
		{
			m_strTemprary = Charset::GB2312ToUnicode(GetPathFromEvr("%tmp%"));
			if (m_strTemprary.empty())
			{
				wchar_t szPath[MAX_PATH] = {0};
				if (0 < ::GetTempPathW(MAX_PATH, szPath))
				{
					m_strTemprary = szPath;
					StringHelper::Trim(m_strTemprary, L'\\');
				}
			}
		}

	return m_strTemprary;
}

wstring PathHelper::GetPathFromCSIDL(const int csidl)
{
	LPITEMIDLIST pIDList = NULL;
	HRESULT hr = SHGetFolderLocation(NULL, csidl, NULL, NULL, &pIDList);
	if (FAILED(hr))
	{
		LOG_TRACE("SHGetFolderLocation FAILED. GetLastError: %d", GetLastError());
		return L"";
	}

	wchar_t szPath[MAX_PATH] = {0};
	hr = SHGetPathFromIDListW(pIDList, szPath);
	if (!SUCCEEDED(hr))
	{
		LOG_TRACE("SHGetPathFromIDListW FAILED. GetLastError: %d", GetLastError());
		ILFree(pIDList);
		return L"";
	}

	ILFree(pIDList);

	return wstring(szPath);
}

wstring PathHelper::GetPathFromEvr(const wstring& name, const wstring& subDir)
{
	wchar_t szPath[MAX_PATH] = {0};
	if (0 == ::GetEnvironmentVariableW(name.c_str(), szPath, MAX_PATH))
	{
		LOG_TRACE("GetEnvironmentVariable FAILED. Environment name: %s; subDir: ", Charset::UnicodeToGB2312(name).c_str(), Charset::UnicodeToGB2312(subDir).c_str());
		return L"";
	}

	wstring path = szPath;
	path += subDir;

	return path;
}

typedef  DWORD(WINAPI *pfnWTSGetActiveConsoleSessionId)(void);
typedef  BOOL(WINAPI *pfnCreateEnvironmentBlock)(LPVOID *lpEnvironment, HANDLE hToken, BOOL bInherit);
typedef  BOOL(WINAPI *pfnDestroyEnvironmentBlock)(LPVOID lpEnvironment);
typedef  BOOL(WINAPI *pfnWTSQueryUserToken)(ULONG SessionId, PHANDLE phToken);
typedef  BOOL (WINAPI *pfnExpandEnvironmentStringsForUser)(HANDLE hToken, LPCSTR lpSrc, LPSTR lpDest, DWORD dwSize);

string PathHelper::GetPathFromEvr(const char* strEvir)
{
	string strDst;
	if (NULL == strEvir || strlen(strEvir) == 0)
	{
		return strDst;
	}

	char szEnv[1024] = {0};
	BOOL bRet = FALSE;
	HANDLE hCurSessionToken = NULL;
	DWORD  dwSessionId = 0;

	pfnWTSGetActiveConsoleSessionId  WTSGetActiveConsoleSessionId = NULL;
	pfnWTSQueryUserToken  WTSQueryUserToken = NULL;
	pfnExpandEnvironmentStringsForUser ExpandEnvironmentStringsForUser = NULL;

	HMODULE hKernel32 = ProcHelper::GetModule(L"Kernel32.dll");
	if(hKernel32 == NULL)
	{
		goto clean;
	}

	HMODULE hWtsapi32 = ProcHelper::GetModule(L"Wtsapi32.dll");
	if(hWtsapi32 == NULL)
	{
		goto clean;
	}

	HMODULE hUserenv = ProcHelper::GetModule(L"Userenv.dll");
	if(hUserenv == NULL)
	{
		goto clean;
	}

	HMODULE hAdvAPI = ProcHelper::GetModule(L"advapi32.dll");
	if(hAdvAPI == NULL)
	{
		goto clean;
	}

	WTSGetActiveConsoleSessionId = (pfnWTSGetActiveConsoleSessionId)GetProcAddress(hKernel32,"WTSGetActiveConsoleSessionId");
	if(WTSGetActiveConsoleSessionId == NULL)
	{
		LOG_TRACE("GetProcAddress <WTSGetActiveConsoleSessionId> FAILED. GetLastError: %d", GetLastError());
		goto clean;
	}

	WTSQueryUserToken = (pfnWTSQueryUserToken)GetProcAddress(hWtsapi32,"WTSQueryUserToken");
	if(WTSQueryUserToken == NULL)
	{
		LOG_TRACE("GetProcAddress <WTSQueryUserToken> FAILED. GetLastError: %d", GetLastError());
		goto clean;
	}

	ExpandEnvironmentStringsForUser = (pfnExpandEnvironmentStringsForUser)GetProcAddress(hUserenv, "ExpandEnvironmentStringsForUserA");
	if(ExpandEnvironmentStringsForUser == NULL)
	{
		LOG_TRACE("GetProcAddress <ExpandEnvironmentStringsForUserA> FAILED. GetLastError: %d", GetLastError());
		goto clean;
	}

	// SE_TCB_NAME
	ProcHelper::MySetPrivilege(SE_TCB_NAME, TRUE);

	dwSessionId = WTSGetActiveConsoleSessionId(); 			
	bRet = WTSQueryUserToken(dwSessionId,&hCurSessionToken);//获取当前Session 登录ID的 Token
	if(bRet)
	{

		bRet = ExpandEnvironmentStringsForUser(hCurSessionToken, (LPCSTR)strEvir, (LPSTR)szEnv, sizeof(szEnv));
		if (bRet != 0)
		{
			strDst = szEnv;
			goto clean;
		}
	}
	else
	{
		LOG_TRACE("WTSQueryUserToken FAILED. GetLastError: %d", GetLastError());
		goto clean;
	}

clean:
	if(strDst.empty())
	{
		bRet = ExpandEnvironmentStringsA(strEvir, szEnv, sizeof(szEnv));
		if (bRet != 0)
		{
			strDst = szEnv;
		}
	}

	CLOSE_HANDLE(hCurSessionToken);

	return strDst;
}


wstring PathHelper::GetKnownFolderPath(const KNOWNFOLDERID& kfid)
{
	typedef HRESULT (WINAPI* fpGetKnownFolderPath)(const GUID& rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);
	fpGetKnownFolderPath getKnownfolderPath = NULL;
	
	wstring strOut;
	const HMODULE hModule = ::LoadLibraryW(L"Shell32.dll");
	if (NULL != hModule)
	{
		return strOut;
	}

	wchar_t* szPath = NULL;
	getKnownfolderPath = (fpGetKnownFolderPath)GetProcAddress(hModule, "SHGetKnownFolderPath");
	if (NULL != getKnownfolderPath)
	{
		if (S_OK != getKnownfolderPath(kfid, 0, 0, &szPath))
		{
			szPath = NULL;
		}
	}

	::FreeLibrary(hModule);

	if (NULL != szPath)
	{	
		strOut = szPath;
		CoTaskMemFree(szPath);
	}

	return strOut;
}

}}
