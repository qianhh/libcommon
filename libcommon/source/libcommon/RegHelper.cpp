#include "stdafx.h"
#include "RegHelper.h"
#include "Charset.h"
#include "log.h"
#include "tchar.h"
#include <vector>
#include <algorithm>
using namespace std;

namespace eric { namespace common {
RegHelper::RegHelper() : m_hPrimaryKey(HKEY_CURRENT_USER)
{
}

RegHelper::~RegHelper()
{
}

void RegHelper::SetPrimaryKey(HKEY hKey)
{
	m_hPrimaryKey = hKey;
}

bool RegHelper::ReadRegValue(const wstring& strSubKey, const wstring& strKeyName, DWORD& dwKeyValue, const DWORD samDesired/* = 0*/)
{
	HKEY hSubKey;
	LSTATUS lStatus = ::RegOpenKeyExW(m_hPrimaryKey, strSubKey.c_str(), 0, KEY_READ | samDesired, &hSubKey);
	if (ERROR_SUCCESS != lStatus)
	{
		LOG_TRACE("RegOpenKeyEx Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
		return false;
	}

    bool bRet = false;

    do 
    {
        wchar_t buf[MAX_PATH * 2] = {0};
        DWORD dwRegType;
        DWORD dwValueSize = MAX_PATH * 2 * sizeof(wchar_t);
        if (ERROR_SUCCESS != RegQueryValueExW(hSubKey, strKeyName.c_str(), NULL, &dwRegType, (LPBYTE)buf, &dwValueSize))
        {
            LOG_TRACE("RegQueryValueEx Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
            break;
        }

        if (dwRegType == REG_DWORD)
        {
            bRet = true;
            dwKeyValue = *((DWORD*)buf);
        }
    } while (false);
	

	RegCloseKey(hSubKey);
    hSubKey = NULL;

	return bRet;
}

bool RegHelper::ReadRegValue(const wstring& strSubKey, const wstring& strKeyName, wstring& strKeyValue, DWORD& dwType, const DWORD samDesired/* = 0*/)
{
	HKEY hSubKey;
	LSTATUS lStatus = ::RegOpenKeyExW(m_hPrimaryKey, strSubKey.c_str(), 0, KEY_READ | samDesired, &hSubKey);
	if (lStatus != ERROR_SUCCESS)
	{
        LOG_TRACE("RegOpenKeyEx Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
		return false;
	}

	dwType = REG_NONE;
	wchar_t* pBuffer = NULL;
    bool bRet = false;

	do 
	{
		DWORD dwRegType = REG_NONE;
		DWORD dwValueSize = 0;
		lStatus = RegQueryValueExW(hSubKey, strKeyName.c_str(), NULL, &dwRegType, NULL, &dwValueSize);
		if (lStatus != ERROR_MORE_DATA && lStatus != ERROR_SUCCESS)
		{
			break;
		}

		pBuffer = new wchar_t[dwValueSize];
		wmemset(pBuffer, 0, dwValueSize);
		lStatus = RegQueryValueExW(hSubKey, strKeyName.c_str(), NULL, &dwRegType, (LPBYTE)pBuffer, &dwValueSize);
		if (lStatus != ERROR_SUCCESS)
		{
			LOG_TRACE("RegQueryValueEx Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
			break;
		}

		if (dwRegType == REG_SZ)
		{
			strKeyValue = pBuffer;
		}
		else if (dwRegType == REG_MULTI_SZ || dwRegType == REG_EXPAND_SZ)
		{
			strKeyValue = wstring(pBuffer, (dwValueSize + 1) / 2);
		}
		dwType = dwRegType;
		bRet = true;
	} while (false);

	DELETE_PTRS(pBuffer);

	RegCloseKey(hSubKey);
	hSubKey = NULL;
	
	return bRet;
}

bool RegHelper::WriteRegValue(const wstring& strSubKey, const wstring& strKeyName, const wstring& strKeyValue, const DWORD dwType, const DWORD samDesired/* = 0*/)
{
	HKEY hCreatKey = NULL;
	LSTATUS lStatus = ::RegCreateKeyExW(m_hPrimaryKey, 
                                        strSubKey.c_str(), 
                                        0, 
                                        NULL, 
                                        REG_OPTION_NON_VOLATILE, 
                                        KEY_READ | KEY_SET_VALUE  | samDesired,
                                        NULL,
                                        &hCreatKey,
                                        NULL);
	if(lStatus != ERROR_SUCCESS)
	{
		LOG_TRACE("RegCreateKeyEx Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
		return false;
	}

	lStatus = ::RegSetValueExW(hCreatKey,
                               strKeyName.c_str(),
                               0,
                               dwType,
                               (const BYTE *)strKeyValue.c_str(),
                               (DWORD)((strKeyValue.length() + 1) * sizeof(wchar_t)));
    bool bRet = true;
	if(lStatus != ERROR_SUCCESS)
	{
		LOG_TRACE("RegSetValueEx Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
		bRet = false;
	}

    ::RegCloseKey(hCreatKey);
	hCreatKey = NULL;

	return bRet;
}

bool RegHelper::WriteRegValue(const wstring& strSubKey, const wstring& strKeyName, const DWORD dwKeyValue, const DWORD samDesired/* = 0*/)
{
	HKEY hCreatKey = NULL;
	LSTATUS lStatus = ::RegCreateKeyExW(m_hPrimaryKey, 
                                        strSubKey.c_str(), 
                                        0, 
                                        NULL,
                                        REG_OPTION_NON_VOLATILE,
                                        KEY_READ | KEY_SET_VALUE | samDesired,
                                        NULL,
                                        &hCreatKey,
                                        NULL);
	if(lStatus != ERROR_SUCCESS)
	{
		LOG_TRACE("RegCreateKeyEx Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
		return false;
	}

	lStatus = ::RegSetValueExW(hCreatKey,
                               strKeyName.c_str(),
                               0,
                               REG_DWORD,
                               (const BYTE *)&dwKeyValue,
                               sizeof(DWORD));
    bool bRet = true;
	if(lStatus != ERROR_SUCCESS)
	{
		LOG_TRACE("RegSetValueEx Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
		bRet =  false;
	}

	::RegCloseKey(hCreatKey);
	hCreatKey = NULL;

	return bRet;
}

bool RegHelper::DeleteRegValue(const wstring& strSubKey, const wstring& strKeyName, const bool isTree, const DWORD samDesired/* = 0*/)
{
	HKEY hSubKey;
	LSTATUS lStatus = ::RegOpenKeyExW(m_hPrimaryKey, strSubKey.c_str(), 0, KEY_SET_VALUE  | samDesired, &hSubKey);
	if (lStatus != ERROR_SUCCESS)
	{
		LOG_TRACE("RegOpenKeyEx Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
		return false;
	}

    bool bRet = true;
	if (isTree)
	{
		lStatus = RegDeleteKey(hSubKey, strKeyName.c_str());
		if (ERROR_SUCCESS != lStatus)
		{
			LOG_TRACE("RegDeleteKey Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
			bRet = false; 
		}
	}
	else
	{
		lStatus = RegDeleteValue(hSubKey, strKeyName.c_str());
		if (ERROR_SUCCESS != lStatus)
		{
            LOG_TRACE("RegDeleteValue Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
			bRet = false;
		}
	}

    ::RegCloseKey(hSubKey);
	hSubKey = NULL;

	return bRet;
}

bool RegHelper::DeleteSubKey(const wstring& strSubKey, const DWORD samDesired)
{
	LSTATUS lStatus = RegDeleteKey(m_hPrimaryKey, strSubKey.c_str());
	if (ERROR_SUCCESS != lStatus)
	{
		 LOG_TRACE("RegDeleteKey Failed. lStatus: %d. strSubKey: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str());
		return false; 
	}

	return true;
}

bool RegHelper::IsRegItemExist(const wstring& strSubKey, const bool isTree/* = true*/, const wstring& strKeyName/* = L""*/, const DWORD samDesired/* = 0*/)
{
	if (strSubKey.empty())
	{
		return false;
	}

	bool bRet = true;
	HKEY hSubKey;
	LSTATUS lStatus = ::RegOpenKeyExW(m_hPrimaryKey, strSubKey.c_str(), 0, KEY_READ | samDesired, &hSubKey);
	if (ERROR_SUCCESS != lStatus)
	{
        LOG_TRACE("RegOpenKeyEx Failed. lStatus: %d. strSubKey: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str());
		return false;
	}

	if (!isTree)
	{
		  wchar_t buf[MAX_PATH * 2] = {0};
		  DWORD dwRegType;
		  DWORD dwValueSize = MAX_PATH * 2 * sizeof(wchar_t);
		  lStatus = RegQueryValueExW(hSubKey, strKeyName.c_str(), NULL, &dwRegType, (LPBYTE)buf, &dwValueSize);
		  if (lStatus != ERROR_SUCCESS)
		  {
              LOG_TRACE("RegQueryValueEx Failed. lStatus: %d. strSubKey: %s; strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strSubKey).c_str(), Charset::UnicodeToGB2312(strKeyName).c_str());
			  bRet = false;
		  }
	}

    ::RegCloseKey(hSubKey);
    hSubKey = NULL;

	return bRet;
}

bool RegHelper::IsRegItemExistEx(const wstring& strSubKey, const bool isTree/* = true*/, const wstring& strKeyName/* = L""*/)
{
	return IsRegItemExist(strSubKey, isTree, strKeyName, KEY_WOW64_32KEY) ||
		   IsRegItemExist(strSubKey, isTree, strKeyName, KEY_WOW64_64KEY);
}

bool RegHelper::EnumLargestKey(const HKEY hRoot, const wstring szKeyName, const wstring filter, wstring & strValue, bool is64And32)
{
	std::vector<wstring> strAllValue;
	if (is64And32)
	{	
		HKEY hSubKey;
		if (::RegOpenKeyExW(hRoot, szKeyName.c_str(), 0, KEY_READ | KEY_WOW64_64KEY, &hSubKey) == ERROR_SUCCESS)
		{
			wchar_t szItem[MAX_PATH];
			DWORD dwItemLen = MAX_PATH;
			int i=0;
			
			while (::RegEnumKeyW(hSubKey, i, szItem, dwItemLen) == ERROR_SUCCESS)
			{
				if (_tcsnicmp(szItem, filter.c_str(), filter.length()) == 0)
				{
					strAllValue.push_back(szItem);
				}
				i++;
				dwItemLen = MAX_PATH;
			}
			::RegCloseKey(hSubKey);
		}
	}
	HKEY hSubKey;
	if (::RegOpenKeyExW(hRoot, szKeyName.c_str(), 0, KEY_READ | KEY_WOW64_32KEY, &hSubKey) == ERROR_SUCCESS)
	{
		wchar_t szItem[MAX_PATH];
		DWORD dwItemLen = MAX_PATH;
		int i=0;
		while (::RegEnumKeyW(hSubKey, i, szItem, dwItemLen) == ERROR_SUCCESS)
		{
			if (_tcsnicmp(szItem, filter.c_str(), filter.length()) == 0)
			{
				strAllValue.push_back(szItem);
			}
			i++;
			dwItemLen = MAX_PATH;
		}
		::RegCloseKey(hSubKey);
	}
	
	if (strAllValue.size() == 1)
		strValue = strAllValue[0];
	else if (strAllValue.size() > 1)
	{
		std::sort(strAllValue.begin(), strAllValue.end());
		strValue = strAllValue[strAllValue.size() - 1];
	}
	

	return strAllValue.size() > 0;
}

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

bool RegHelper::EnumAllKey(const wstring& strKeyName,vector<wstring>& vecTreeKey,vector<wstring>& vecSubKey) 
{ 
	if (strKeyName.empty())
	{
		return false;
	}

	wchar_t    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	wchar_t    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys=0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode; 

	wchar_t  achValue[MAX_VALUE_NAME]; 
	DWORD cchValue = MAX_VALUE_NAME; 

	HKEY hTestKey;
	LSTATUS lStatus;
	if( (lStatus = RegOpenKeyEx( m_hPrimaryKey, strKeyName.c_str(), 0, KEY_READ, &hTestKey)) != ERROR_SUCCESS)
	{
		LOG_TRACE("QueryAllKey::RegOpenKeyEx Failed. lStatus: %d. strKeyName: %s", lStatus, Charset::UnicodeToGB2312(strKeyName).c_str());
		return false;
	}

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hTestKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

	// Enumerate the subkeys, until RegEnumKeyEx fails.

	if (cSubKeys)
	{
		for (i=0; i < cSubKeys; i++) 
		{ 
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hTestKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime); 
			if (retCode == ERROR_SUCCESS) 
			{
				vecTreeKey.push_back(achKey);
			}
		}
	} 
	// Enumerate the key values. 
	if (cValues) 
	{
		for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++) 
		{ 
			cchValue = MAX_VALUE_NAME; 
			achValue[0] = L'\0'; 
			retCode = RegEnumValue(hTestKey, i, achValue, &cchValue, NULL, NULL, NULL, NULL);

			if (retCode == ERROR_SUCCESS ) 
			{ 
				vecSubKey.push_back(achValue);
			} 
		}
	}

	RegCloseKey(hTestKey);
	return true;
}

}}
