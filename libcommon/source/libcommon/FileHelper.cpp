#include "stdafx.h"
#include "FileHelper.h"
#include <Windows.h>
#include "log.h"
#include <string>
#include "Charset.h"
#include "StringHelper.h"
#include "PathHelper.h"
#include <ShellAPI.h>
#include "Shlwapi.h"
#pragma comment(lib,"Shlwapi.lib")
using namespace std;

namespace eric { namespace common {
bool FileHelper::IsDirPath(const wstring& strPath)
{
    if (strPath.empty())
    {
        return false;
    }

	wstring tmpPath = strPath;
	StringHelper::RTrim(tmpPath, L'/');
	StringHelper::RTrim(tmpPath, L'\\');

	// 先判断是否是驱动器
	if (tmpPath.length() <= 3)
	{
		 UINT nRes = ::GetDriveTypeW(tmpPath.c_str()); 
		 switch (nRes)
		 {
		 case DRIVE_UNKNOWN:		// 未知磁盘
		 case DRIVE_NO_ROOT_DIR:	// 相对路径
		 case DRIVE_REMOVABLE:		// 已被移除的磁盘
			 LOG_TRACE("GetDriveTypeW FAILED. GetLastError[%d]:%s. tmpPath: %s", GetLastError(), GetLastErrorMessage().c_str(), Charset::UnicodeToGB2312(tmpPath).c_str());
			 return false;
		 	
		 case DRIVE_FIXED:			// 本地磁盘
		 case DRIVE_REMOTE:			// 远程磁盘
		 //case DRIVE_CDROM:		// CDROM		
		 //case DRIVE_RAMDISK:		// RAM
			 return true;
		 } 
	}

	WIN32_FIND_DATAW wfd;
	HANDLE hFile = FindFirstFileW(tmpPath.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		// 如果路径不存在，通过路径字符串查找是否有/，\来判断是不是合法的路径
		const size_t length = strPath.length() - 1;
		if (strPath.at(length) == L'\\' || 
			strPath.at(length) == L'/')
		{
			// 对不存在的文件目录 以 //结尾 并且不存在，输出日志会引起崩溃 2015.4.20
			//DEBUGW(DBG_ERROR, L"FindFirstFileW failed, check path ends with" << strPath);
			return true;
		}
		else
		{
			return false;
		}		
	}

	if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		FindClose(hFile);
		return true;
	}

	FindClose(hFile);
	return false;
}

bool FileHelper::IsFileOrDirExist(const wstring& strFile)
{
    if (strFile.empty())
    {
        return false;
    }

	return PathFileExistsW(strFile.c_str()) ? true : false;
}

bool FileHelper::SplitDirectoryAndFileName(const wstring strFullPath, wstring& strDirectory, wstring& strFileName)
{
	if (strFullPath.empty())
	{
		return false;
	}

	if (IsDirPath(strFullPath))
	{
        strDirectory = strFullPath;
	}
	else
	{
        wchar_t drive[MAX_PATH] = {0};
        wchar_t dir[MAX_PATH] = {0};
        wchar_t fname[MAX_PATH] = {0};
        wchar_t ext[MAX_PATH] = {0};
		if (EINVAL == _wsplitpath_s(strFullPath.c_str(), drive, dir, fname, ext))
		{
			return false;
		}
		
        strDirectory = (wstring)drive + dir;
        strFileName = (wstring)fname + ext;
	}

	if (strDirectory.empty())
	{
		return false;
	}

	try
	{
		const size_t len = strDirectory.length() - 1;
		if (strDirectory.at(len) != L'\\' && 
			strDirectory.at(len) != L'/')
		{
			strDirectory += L"\\";
		}
	}
	catch (exception e)
	{
		LOG_TRACE("Catch EXCEPTION. strDirectory: %s", Charset::UnicodeToGB2312(strDirectory).c_str());
		return false;
	}
	
	
	return true;
}

static size_t ParsePath(const wstring& str, vector<wstring>& vec)
{
    if (str.empty())
    {
        return 0;
    }

	wstring strTemp(str);
	StringHelper::Replace(strTemp, L"/", L"\\");

	static int nTokenLen = 1;
	wstring strRecord;
	size_t nLastPos = 0;
	size_t nPos = strTemp.find(L"\\", 0);
	while(nPos != string::npos)
	{
		strRecord = strTemp.substr(0, nPos);
		vec.push_back(strRecord);
		nLastPos = nPos + nTokenLen;
		nPos = strTemp.find(L"\\", nLastPos);
	}
	if (nLastPos != strTemp.size())
	{
		vec.push_back(str);
	}
	return vec.size();
}

bool FileHelper::CreateDir(const wstring& strPath)
{
    if (strPath.empty())
    {
        return false;
    }

	wstring strPathTmp = strPath;
    if (strPathTmp.at(strPathTmp.length() - 1) != L'\\'&& 
        strPathTmp.at(strPathTmp.length() - 1) != L'/')
	{
		strPathTmp += L"\\";
	}

	if (!IsDirPath(strPathTmp))
	{
		LOG_TRACE("NOT DIRPATH: %s", Charset::UnicodeToGB2312(strPath).c_str());
		return false;
	}

	if (IsFileOrDirExist(strPathTmp))
	{
		return true;
	}

	vector<wstring> vecPath;
	ParsePath(strPathTmp, vecPath);
	if (vecPath.empty())
	{
		LOG_TRACE("PARTH DIR FAILED: %s", Charset::UnicodeToGB2312(strPath).c_str());
		return false;
	}
	bool bRet(true);
	for (vector<wstring>::iterator it = vecPath.begin(); it != vecPath.end(); ++it)
	{
		const wstring& strTmp = (*it);
		if (!IsFileOrDirExist(strTmp.c_str()))
		{
			if (!CreateDirectoryW(strTmp.c_str(), NULL))
			{
				LOG_TRACE("Create directory[%s] FAILED, error: %s", Charset::UnicodeToGB2312(strTmp).c_str(), GetLastErrorMessage().c_str());
				bRet = false;
				break;
			}
		}
	}

	return bRet;
}

wstring FileHelper::GenerateTempRandomDir()
{
    wstring strDir = PathHelper::GetInstance()->GetTemporaryPath();
    if (strDir.empty())
    {
        return strDir;
    }
    
    DWORD dwRandom = GetCurrentTime();
    TCHAR szName[MAX_PATH] = {0};
    swprintf_s(szName, MAX_PATH, L"%x", GetCurrentTime());

    strDir.append(L"\\").append(szName);
    
    return CreateDir(strDir) ? strDir : L"";
}

// bAllowUndo - true: 允许放入回收站；- false：不允许放入回收站。
bool FileHelper::DeleteDir(const wstring& strPath, bool bAllowUndo/* = false*/)
{
    /**
     ** !!!在XP系统下，如果路径末尾包含空格，SHFileOperationW会调用失败，总是返回：6
     ** 所以在这里需要去掉路径的空格
     **/
    wstring strTmp = strPath;
    StringHelper::Trim(strTmp, L' ');

	if (!IsFileOrDirExist(strTmp))  //路径不存在或不是路径，表示删除成功
	{
		return true;
	}
	// 确保目录的路径以2个\0结尾???
	wchar_t* pszFrom = new wchar_t[strTmp.length() + 2];
	//wcscpy_s(pszFrom, wcslen(pszFrom), strPath.c_str());
    wcscpy_s(pszFrom,strTmp.length() + 2,strTmp.c_str());
	pszFrom[strTmp.length()] = L'\0';
	pszFrom[strTmp.length() + 1] = L'\0';

	SHFILEOPSTRUCTW fo; 
	ZeroMemory(&fo, sizeof(SHFILEOPSTRUCTW)); 
	fo.fFlags |= FOF_SILENT;
	fo.fFlags |= FOF_NOERRORUI;
	fo.fFlags |= FOF_NOCONFIRMATION;
	fo.hNameMappings = NULL;
	fo.hwnd = NULL;
	fo.lpszProgressTitle = NULL;
	fo.wFunc = FO_DELETE;
	fo.pFrom = pszFrom;
	fo.pTo = NULL; 

	if (bAllowUndo)
	{   
		fo.fFlags |= FOF_ALLOWUNDO;
	}  
	else  
	{   
		fo.fFlags &= ~FOF_ALLOWUNDO;
	}

	bool bRt = true;
	if (0 != SHFileOperationW(&fo))
	{
		LOG_TRACE("SHFileOperation Failed. GetLastError[%d]: %s. strPath: %s", GetLastError(), GetLastErrorMessage().c_str(), Charset::UnicodeToGB2312(strTmp).c_str());
		bRt = false;
	}

    DELETE_PTRS(pszFrom);
	return bRt;
}

bool FileHelper::EmptyDir(const wstring& strPath)
{
    wstring strTmp = strPath;
    StringHelper::Trim(strTmp, L' ');

	if(!IsFileOrDirExist(strTmp))
	{
		return false;
	}

	WIN32_FIND_DATAW wfd;
	HANDLE hFind;
	wstring sFindFilter = strTmp + L"\\*.*";
	if ((hFind = FindFirstFileW(sFindFilter.c_str(), &wfd)) == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	do
	{
		//这两个目录不能被删除
		if(_wcsicmp(L".",wfd.cFileName) == 0
			|| _wcsicmp(L"..",wfd.cFileName) == 0)
		{
			continue;
		}
		wstring sFullPath = strTmp + L'\\' + wfd.cFileName;
		//去掉只读属性
		DWORD dwAttributes = GetFileAttributesW(sFullPath.c_str());
		if (dwAttributes & FILE_ATTRIBUTE_READONLY)
		{
			dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributesW(sFullPath.c_str(), dwAttributes);
		}
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			EmptyDir(sFullPath.c_str());
			RemoveDirectoryW(sFullPath.c_str());
		}
		else
		{
			DelFile(sFullPath.c_str(),true);
		}
	} while (FindNextFileW(hFind, &wfd));

	FindClose(hFind);

	return true;
}

bool FileHelper::DelFile(const wstring& strFile,bool delReadonly)
{
	if (!IsFileOrDirExist(strFile))
	{
		return true;
	}

	// 如果是路径，直接退出
	if (IsDirPath(strFile))
	{
		return false;
	}

	if (!::DeleteFileW(strFile.c_str()) && delReadonly)
	{
		DWORD dwAttributes = GetFileAttributesW(strFile.c_str());
		if (dwAttributes & FILE_ATTRIBUTE_READONLY)
		{
			dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributesW(strFile.c_str(), dwAttributes);
		}
		if(!DeleteFileW(strFile.c_str()))
		{
			return RebootRemoveFile(strFile);
		}		
	}
	return true;
}

bool FileHelper::DeleteFileOrDir(const wstring& strDstPath, const bool bAllowUndo)
{
    if (strDstPath.empty())
    {
        return true;
    }

	DWORD value = GetFileAttributes(strDstPath.c_str());
	if(value & FILE_ATTRIBUTE_DIRECTORY)
	{
        EmptyDir(strDstPath);
		return DeleteDir(strDstPath, bAllowUndo);
	}
	return DelFile(strDstPath, bAllowUndo);
}

bool FileHelper::RebootRemoveFile(const wstring& strFile)
{
	wchar_t buffer[64] = {0};
	wsprintfW(buffer, L"%x", ::GetTickCount());
	wstring strResult;
	size_t pos = wstring::npos;
	if ((pos = strFile.find(L".tmp")) != wstring::npos)
	{
		strResult = strFile.substr(0, pos+wcslen(L".tmp")) + buffer;
	}
	else
	{
		strResult = strFile + L".tmp" + buffer;
	}
	if (::MoveFileW(strFile.c_str(), strResult.c_str()))
	{
		LOG_TRACE("MoveFileEx MOVEFILE_DELAY_UNTIL_REBOOT. strResult: %s", Charset::UnicodeToGB2312(strResult).c_str());
		if (!::MoveFileExW(strResult.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
		{
			LOG_TRACE("MoveFileExW GetLastError[%d]: %s", GetLastError(), GetLastErrorMessage().c_str());
			return false;
		}
		return true;
	}

	return false;
}

bool FileHelper::CreateFileAlways(const wstring &strFileName)
{
	if (strFileName.empty())
	{
		return false;
	}	
	wstring dir;
	wstring name;
	if (!SplitDirectoryAndFileName(strFileName, dir, name))
	{
		LOG_TRACE("SplitDirectoryAndFileName FAILED. strFileName: %s", Charset::UnicodeToGB2312(strFileName).c_str());
		return false;
	}

	if (!CreateDir(dir))
	{
        LOG_TRACE("CreateDir FAILED. dir: %s", Charset::UnicodeToGB2312(dir).c_str());
		return false;
	}

    bool bRet = true;
	HANDLE hFile = CreateFileW(strFileName.c_str(), GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
	if (INVALID_HANDLE_VALUE == hFile) 
	{ 
		LOG_TRACE("CreateFileW FAILED. GetLastError[%d]: %s, strFileName: %s", GetLastError(), GetLastErrorMessage().c_str(), Charset::UnicodeToGB2312(strFileName).c_str());
		bRet = false;
	}  	
	
    CLOSE_HANDLE(hFile);
	return bRet;
}

}}
