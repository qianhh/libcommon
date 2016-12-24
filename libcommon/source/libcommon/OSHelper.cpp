#include "stdafx.h"
#include "OSHelper.h"
#include "FileHelper.h"
#include "StringHelper.h"
#include "ProcHelper.h"
#include "Charset.h"
#include "common.h"
#include "log.h"
#include <Sddl.h>

using namespace std;

namespace eric { namespace common {
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

bool OSHelper::IsWow64(HANDLE hProc/* = NULL*/)
{
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"), "IsWow64Process");
	if(NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(hProc == NULL ? GetCurrentProcess() : hProc, &bIsWow64))
		{
			bIsWow64 = FALSE;
		}
	}

	return bIsWow64 ? true : false;
}

bool OSHelper::Is64Proc(HANDLE hProc)
{
	SYSTEM_INFO sysinfo;
	GetNativeSystemInfo(&sysinfo);
	if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 || sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
    {
		return !IsWow64(hProc);
    }

	return false;
}

OSHelper::WindowsVersion OSHelper::GetWindowsVersion()
{
	OSVERSIONINFOW osinfo;
	ZeroMemory( &osinfo, sizeof(OSVERSIONINFOW) );
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);

	if (GetVersionExW(&osinfo) )
	{
		DWORD dwMajorVer = osinfo.dwMajorVersion;
		DWORD dwMinorVer = osinfo.dwMinorVersion;

		if ( 5 == dwMajorVer && 1 == dwMinorVer )
		{
			return WinXP;
		}

		if ( 6 == dwMajorVer && 0 == dwMinorVer )
		{
			return WinVista;
		}

		if ( 6 == dwMajorVer && 1 == dwMinorVer )
		{
			return Win7;
		}

		if ( 6 == dwMajorVer && 2 == dwMinorVer )
		{
			return Win8;
		}
	}

	return UnknownVer;
}

#define BUFSIZE 256

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

static bool GetOSDisplayString(string& strOS)
{
	OSVERSIONINFOEXA osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	BOOL bOsVersionInfoEx;
	DWORD dwType;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
	bOsVersionInfoEx = GetVersionExA((OSVERSIONINFOA*)&osvi);
	if (!bOsVersionInfoEx)
    {
        return false;
    }

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
	pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if(NULL != pGNSI)
	{
		pGNSI(&si);
	}
	else
	{
		GetSystemInfo(&si);
	}

	if ( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4 )
	{
		strOS = "Microsoft ";

		// Test for the specific product.
		if ( osvi.dwMajorVersion == 6 )
		{
			if( osvi.dwMinorVersion == 0 )
			{
				if( osvi.wProductType == VER_NT_WORKSTATION )
				{
					strOS += "Windows Vista ";
				}
				else
				{
					strOS += "Windows Server 2008 ";
				}
			}

			if ( osvi.dwMinorVersion == 1 || osvi.dwMinorVersion == 2 )
			{
				if ( osvi.wProductType == VER_NT_WORKSTATION && osvi.dwMinorVersion == 1)
				{
					strOS += "Windows 7 ";
				}
				else
				{
					if ( osvi.wProductType == VER_NT_WORKSTATION && osvi.dwMinorVersion == 2)
					{
						strOS += "Windows 8 ";
					}
					else
					{
						strOS += "Windows Server 2008 R2 ";
					}
				}
			}

			pGPI = (PGPI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
			pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

			switch( dwType )
			{
			case PRODUCT_ULTIMATE:
				strOS += "Ultimate Edition";
				break;

			case PRODUCT_PROFESSIONAL:
				strOS += "Professional";
				break;

			case PRODUCT_HOME_PREMIUM:
				strOS += "Home Premium Edition";
				break;

			case PRODUCT_HOME_BASIC:
				strOS += "Home Basic Edition";
				break;

			case PRODUCT_ENTERPRISE:
				strOS += "Enterprise Edition";
				break;

			case PRODUCT_BUSINESS:
				strOS += "Business Edition";
				break;

			case PRODUCT_STARTER:
				strOS += "Starter Edition";
				break;

			case PRODUCT_CLUSTER_SERVER:
				strOS += "Cluster Server Edition";
				break;

			case PRODUCT_DATACENTER_SERVER:
				strOS += "Datacenter Edition";
				break;

			case PRODUCT_DATACENTER_SERVER_CORE:
				strOS += "Datacenter Edition (core installation)";
				break;

			case PRODUCT_ENTERPRISE_SERVER:
				strOS += "Enterprise Edition";
				break;

			case PRODUCT_ENTERPRISE_SERVER_CORE:
				strOS += "Enterprise Edition (core installation)";
				break;

			case PRODUCT_ENTERPRISE_SERVER_IA64:
				strOS += "Enterprise Edition for Itanium-based Systems";
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				strOS += "Small Business Server";
				break;

			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				strOS += "Small Business Server Premium Edition";
				break;

			case PRODUCT_STANDARD_SERVER:
				strOS += "Standard Edition";
				break;

			case PRODUCT_STANDARD_SERVER_CORE:
				strOS += "Standard Edition (core installation)";
				break;

			case PRODUCT_WEB_SERVER:
				strOS += "Web Server Edition";
				break;

			default:
				strOS += "Unknown Product";
				break;
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			if( GetSystemMetrics(SM_SERVERR2) )
			{
				strOS += "Windows Server 2003 R2, ";
			}
			else if ( osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER )
			{
				strOS += "Windows Storage Server 2003";
			}
			else if ( osvi.wSuiteMask & VER_SUITE_WH_SERVER )
			{
				strOS += "Windows Home Server";
			}
			else if( osvi.wProductType == VER_NT_WORKSTATION &&	si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
			{
				strOS += "Windows XP Professional x64 Edition";
			}
			else
			{
				strOS += "Windows Server 2003, ";
			}

			// Test for the server type.
			if ( osvi.wProductType != VER_NT_WORKSTATION )
			{
				if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					{
						strOS += "Datacenter Edition for Itanium-based Systems";
					}
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					{
						strOS += "Enterprise Edition for Itanium-based Systems";
					}
				}
				else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					{
						strOS += "Datacenter x64 Edition";
					}
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					{
						strOS += "Enterprise x64 Edition";
					}
					else 
					{
						strOS += "Standard x64 Edition";
					}
				}
				else
				{
					if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
					{
						strOS += "Compute Cluster Edition";
					}
					else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					{
						strOS += "Datacenter Edition";
					}
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					{
						strOS += "Enterprise Edition";
					}
					else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
					{
						strOS += "Web Edition";
					}
					else
					{
						strOS += "Standard Edition";
					}
				}
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			strOS += "Windows XP ";
			if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
			{
				strOS += "Home Edition";
			}
			else
			{
				strOS += "Professional";
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			strOS += "Windows 2000 ";
			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				strOS += "Professional";
			}
			else 
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
				{
					strOS += "Datacenter Server";
				}
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
				{
					strOS += "Advanced Server";
				}
				else
				{
					strOS += "Server";
				}
			}
		}

		// Include service pack (if any) and build number.
		std::string strVersion = osvi.szCSDVersion;
		if( strVersion.length() > 0 )
		{
			strOS += " ";
			strOS += strVersion;
		}

		char buf[80];
		sprintf_s( buf, " (build %d)", osvi.dwBuildNumber);
		strOS += buf;

		if ( osvi.dwMajorVersion >= 6 )
		{
			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
			{
				strOS += ", 64-bit";
			}
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
			{
				strOS += ", 32-bit";
			}
		}

		return true; 
	}
	else
	{  
        LOG_TRACE("This sample does not support this version of Windows.");
		return false;
	}
}

wstring OSHelper::GetOSDetailInfo()
{
	string strResult;
	return GetOSDisplayString(strResult) ? Charset::GB2312ToUnicode(strResult) : L"";
}

bool OSHelper::IsVistaOrHigher()
{
	WindowsVersion wndVer = GetWindowsVersion();
	if (WinXP == wndVer)
	{
		return false;
	}
	else if (WinVista == wndVer || Win7 == wndVer || Win8 == wndVer)
	{
		return true;
	}

	return true;
}

void OSHelper::SendMsgOnVistaOrHigher(UINT msgID)
{
    // Only support in Vista or Higher OS.
    if (OSHelper::IsVistaOrHigher())
    {
        typedef BOOL (WINAPI FAR *ChangeWindowMessageFilter_PROC)(UINT, DWORD);
        ChangeWindowMessageFilter_PROC m_pfnChangeWindowMessageFilter; 
        m_pfnChangeWindowMessageFilter = (ChangeWindowMessageFilter_PROC)::GetProcAddress(::GetModuleHandle(L"USER32"), "ChangeWindowMessageFilter"); 
        if (m_pfnChangeWindowMessageFilter) 
        { 
            m_pfnChangeWindowMessageFilter(msgID, 1/*Add*/); 
        }
    }
}

wstring OSHelper::GetDiskInfo(const wstring& disk/* = L"C:"*/)
{
	wstring strRet;

	_ULARGE_INTEGER lpFreeBytesAvailableToCaller;
	_ULARGE_INTEGER lpTotalNumberOfBytes;
	_ULARGE_INTEGER lpTotalNumberOfFreeBytes;

	if(GetDriveTypeW(disk.c_str()) == DRIVE_FIXED)
	{  
		GetDiskFreeSpaceExW(disk.c_str(), &lpFreeBytesAvailableToCaller, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);

		double dTmp = lpTotalNumberOfFreeBytes.QuadPart / (1024.0 * 1024.0 *1024.0);
		wchar_t szBuf[MAX_PATH] = {0};
		swprintf_s(szBuf, L"%0.2lf", dTmp);

		strRet = szBuf;
	}

	return strRet;
}

int OSHelper::GetIntegrityLevel()
{
    if (!OSHelper::IsVistaOrHigher())
    {
		// XP及以下都是管理员权限
        return 3;
    }

    if (ProcHelper::IsProcessRunAsAdmin())
    {
		// 已经以管理员权限运行
        return 3;
    }

    DWORD dwExplorer = ProcHelper::GetProcessIDByName(L"Explorer.exe");
    if (dwExplorer > 0)
    {
        HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwExplorer);
        if(NULL != hProc)
        {
			// 能打开资源浏览器，表示为用户权限
            CLOSE_HANDLE(hProc);
            return 2;
        }
    }

	// 否则返回低权限
    return 1;
}

bool OSHelper::CreateSecurityDescriptor(SECURITY_ATTRIBUTES& sa, SECURITY_DESCRIPTOR& sd, const wstring& strSD/* = L""*/)
{
	bool bRet = false;

	sa.nLength = sizeof(sa);  
	sa.bInheritHandle = FALSE;  
	sa.lpSecurityDescriptor = &sd;

	bRet = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION) ? true : false;
	if (!bRet)
	{
		LOG_TRACE("InitializeSecurityDescriptor FAILED. GetLastError: %d", GetLastError());
		return bRet;
	}

	// DACL
	bRet = SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE) ? true : false;
	if (!bRet)
	{
		LOG_TRACE("InitializeSecurityDescriptor FAILED. GetLastError: %d", GetLastError());
		return bRet;
	}

	// 1.如果是XP系统，到这里直接退出即可。
	if (!IsVistaOrHigher())
	{
		return bRet;
	}

	// 2.走到这里，说明是Vista及以上版本，需要进一步解析安全描述符字符串
	if (!strSD.empty())
	{
		PSECURITY_DESCRIPTOR pSD = NULL;

		bRet = ConvertStringSecurityDescriptorToSecurityDescriptorW(strSD.c_str(), SDDL_REVISION_1, &pSD, NULL) ? true : false;
		if (!bRet)
		{
			LOG_TRACE("ConvertStringSecurityDescriptorToSecurityDescriptorW FAILED. GetLastError: %d", GetLastError());
			return bRet;
		}

		PACL pSacl = NULL;
		BOOL bSaclPresent = FALSE;
		BOOL bSaclDefaulted = FALSE;
		bRet = GetSecurityDescriptorSacl(pSD, &bSaclPresent, &pSacl, &bSaclDefaulted) ? true : false;
		if (!bRet)
		{
			LOG_TRACE("GetSecurityDescriptorSacl FAILED. GetLastError: %d", GetLastError());
			return bRet;
		}

		bRet = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION) ? true : false;
		if (!bRet)
		{
			LOG_TRACE("InitializeSecurityDescriptor FAILED. GetLastError: %d", GetLastError());
			return bRet;
		}

		// SACL
		bRet = SetSecurityDescriptorSacl(&sd, TRUE, pSacl, FALSE) ? true : false;
		if (!bRet)
		{
			LOG_TRACE("InitializeSecurityDescriptor FAILED. GetLastError: %d", GetLastError());
			return bRet;
		}
	}

	return bRet;
}

}}
