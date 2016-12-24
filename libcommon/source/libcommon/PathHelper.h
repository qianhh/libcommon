#pragma once
#include "Singleton.h"
#include <ShlObj.h>

#define REG_KEY_DISPLAY_VERSION     L"DisplayVersion"
#define REG_KEY_UNINSTALL_STRING    L"UninstallString"
#define REG_KEY_DISPLAY_ICON        L"DisplayIcon"
#define REG_KEY_INST_LOCATION       L"InstallLocation"
#define REG_ITEM_SHELL_FOLDER		L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
#define REG_ITEM_UNINSTALL_32BIT	L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
#define REG_ITEM_UNINSTALL_64BIT	L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"

/**
 ** 路径管理类
 ** designed by eric,2016/11/15
 **/
namespace eric { namespace common {
class PathHelper : public CSingleton<PathHelper>
{
	friend class CSingleton<PathHelper>;
public:
	/**
     ** 获取Local AppData路径
	 ** Win7 C:\Users\用户名\AppData\Local
	 ** WinXP C:\Documents and Settings\用户名\Local Settings\Application Data
     ** @return                 Local路径
     **/
    std::wstring GetLocalPath();
    /**
     ** 获取LocalLow路径
	 ** Win7 C:\Users\lscm\AppData\LocalLow
	 ** WinXP 不存在
     ** @return                 LocalLow路径
     **/
    std::wstring GetLocalLowPath();
    /**
     ** 获取Roaming路径
	 ** Win7 C:\Users\用户名\AppData\Roaming
	 ** WinXP C:\Documents and Settings\用户名\Application Data
     ** @return                 Roaming路径
     **/
    std::wstring GetRoamingPath();
    /**
     ** 从环境变量中获取AllUserAppData路径,有可能失败
	 ** Win7 C:\ProgramData
	 ** WinXP C:\Documents and Settings\All Users
     ** @return                 AllUserAppData路径
     **/
    std::wstring GetAllUserAppDataPath();
    /**
     ** 获取Desktop路径
     ** @return                 Desktop路径
     **/
	std::wstring GetDesktopPath();
	/**
     ** 获取Startmenu路径
     ** @return                 Startmenu路径
     **/
    std::wstring GetStartmenuPath();
	/**
     ** 获取临时目录tmp路径
     ** Win7 C:\Users\用户名\AppData\Local\Temp
     ** WinXP C:\Documents and Settings\用户名\Local Settings\Temp
     ** @return                 临时目录的路径
     **/
    std::wstring GetTemporaryPath();    
    /**
     ** 获取Program Files的路径
     ** @return                 Program Files的路径
     **/
    std::wstring GetProgramFilesPath();
    /**
     ** 获取Program Files(X86)的路径
	 ** Win7 C:\Program Files (x86)
	 ** WinXP C:\Program Files
     ** @return  Program Files(X86)的路径
     **/
    std::wstring GetProgramFilesX86Path();
    /**
     ** 获取系统盘符
     ** @return                 系统盘符字符串
     **/
    std::wstring GetSystemDriver();
	/**
     ** 从注册表中获取路径
     ** @param[in] hKey         注册表键值，比如：HKEY_CURRENT_USER
     ** @param[in] subKey       注册表的子项，比如：Software\Microsoft\Windows\CurrentVersion\Uninstall\365天气
     ** @param[in] name         需要查找的键值，比如：DisplayIcon
	 ** @param[in] boSamDesired	注册表Wow6432Node和普通的，两个都读或只读一个（对应到RegReadValue的samDesired项）
     ** @return                 安装目录的路径，比如：C:\Users\xiaof\AppData\Local\365Weather\Application\Default\365Weather.exe
     **/
    std::wstring GetPathFromReg(const HKEY& hKey, const std::wstring& subKey, const std::wstring& name, bool boSamDesired = false);

    /**
     ** 从CSIDL获取路径，比如：CSIDL_LOCAL_APPDATA表示Local路径
     ** @param[in] csidl        CSIDL值
     ** @return                 路径字符串
     **/
    std::wstring GetPathFromCSIDL(const int csidl);
    /**
     ** 从环境变量中获取路径
     ** @param[in] name         环境变量的名字，比如：ALLUSERSPROFILE表示Program Data的路径
     ** @param[in] subDir       子文件夹
     ** @return                 路径字符串
     **/
    std::wstring GetPathFromEvr(const std::wstring& name, const std::wstring& subDir);
    /**
     ** 从环境变量中获取路径
     ** @param[in] strEvir      环境变量的名字，比如：%ALLUSERSPROFILE%表示Program Data的路径
     ** @return                 路径字符串
     **/
    std::string GetPathFromEvr(const char* strEvir);
	/**
     ** 从系统中获取被注册为Known Folders的文件夹路径
     ** @return                 文件夹路径
     **/
    std::wstring GetKnownFolderPath(const KNOWNFOLDERID& kfid);
private:
	PathHelper();
    ~PathHelper();

private:
    std::wstring m_strLocal;         // Local路径
    std::wstring m_strLocalLow;      // LocalLow路径
    std::wstring m_strRoaming;       // Roaming路径
    std::wstring m_strAllUser;       // ProgramData路径
    std::wstring m_strDesktop;       // Desktop路径
    std::wstring m_strStartmenu;     // Startmenu路径
    std::wstring m_strTemprary;      // Temp路径
    std::wstring m_strProgramFiles;  // ProgramFiles路径
    std::wstring m_strInstall;       // 程序的安装路径
    std::wstring m_strConfig;        // 配置文件路径
    std::wstring m_strUserName;      // 用户名
    bool    m_isSysProc;        // 是否为用户进程
};

}}
