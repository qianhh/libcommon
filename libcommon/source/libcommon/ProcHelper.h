#pragma once

/**
 ** 进程操作的相关类
 ** designed by eric,2016/08/23
 **/
namespace eric { namespace common {
class ProcHelper
{
public:
    /**
     ** 设置进程单例运行，避免启动多个同样的EXE
     ** @param[in] pszProcName				进程名
     ** @retval								true 设置成功; false 设置失败，可能已存在该进程
     **/
    static bool SetSigletonProc(const wchar_t *pszProcName);
	/**
     ** 创建进程
     ** @param[in] cmd          命令行
     ** @param[in] appName      应用程序名字
     ** @param[in] workingDir   当前工作路径
     ** @param[in] bShowWindow  是否显示窗口
     ** @param[in] bInherit     是否继承父进程
     ** @param[in] bWait        是否支持异步等待
     ** @param[in] dwWaitTime   等待时间
     ** @param[in] pThreadID    线程ID
     ** @param[in] pProcessID   进程ID
     ** @param[in] pExitCode    进程退出码
     ** @retval                 true 成功
     ** @retval                 false 失败
     **/
	static bool CreateProcessInherit(const std::wstring& cmd, 
									 const std::wstring& appName = L"",
									 const std::wstring& workingDir = L"",
									 const bool bShowWindow = false,
									 const bool bInherit = true,
									 const bool bWait = false,
									 DWORD dwWaitTime = 1000,
									 DWORD* pThreadID = NULL,
									 DWORD* pProcessID = NULL,
									 DWORD* pExitCode = NULL);
	/**
     ** 创建用户进程
     ** @param[in] path         进程的路径
     ** @param[in] cmd			命令行
     ** @param[in] bShowWnd		是否显示窗口，true-显示、false-不显示
     ** @retval                 true 成功
     ** @retval                 false 失败
     **/
    static bool MyCreateProcessAsUser(const std::wstring& path, const std::wstring& cmd = L"", const bool bShowWnd = false);
	/**
     ** 获取当前进程的全路径
     ** @return								进程的名字
     **/
	static std::wstring GetCurrentProcessPathName(void);
	/**
     ** 获取当前进程所在的目录
     ** @return								进程的路径
     **/
    static std::wstring GetCurrentProcessPath(void);
	/**
     ** 获取当前进程的名字
     ** @return								进程的名字
     **/
	static std::wstring GetCurrentProcessName(void);
	 /**
     ** 获取当前函数的module
     ** @retval								module值
     **/
	static HMODULE GetCurrentModule(void);
	/**
     ** 获取当前module的全路径
     ** @return								module的路径
     **/
	static std::wstring GetCurrentModulePathName(void);
	/**
     ** 获取当前module的目录
     ** @return								module的路径
     **/
	static std::wstring GetCurrentModulePath(void);
	/**
     ** 获取当前module的名字
     ** @return								module的名字
     **/
	static std::wstring GetCurrentModuleName(void);
	/**
     ** 注册DLL
     ** @param[in] dllPath					目标DLL的路径
     ** @retval								true 成功
     ** @retval								false 失败
     **/
	static bool RegisterDLL(const std::wstring& dllPath, bool bSilent = true);
    /**
     ** 注销DLL
     ** @param[in] dllPath					目标DLL的路径
     ** @retval								true 成功
     ** @retval								false 失败
     **/
	static bool UnregisterDLL(const std::wstring& dllPath, bool bSilent = true);
	/**
     ** 获取指定名称的模块
     ** @param[in] modulePath				模块的名称
     ** @return								指定模块的标志
     **/
	static HMODULE GetModule(const std::wstring& modulePath);
	/**
     ** 当前权限启动进程
     ** @param[in] filePath					进程可执行文件
	 ** @retval								true 成功
	 ** @retval								false 失败
     **/
	static bool LaunchProcess(const std::wstring& filePath);
	/**
     ** 以user权限启动进程
     ** @param[in] strCmdLine				命令行，比如：C:\XX.exe Cmd=start
     ** @param[in] strFileName				进程可执行文件，默认为空
     ** @param[in] bShowWnd					是否显示程序界面，默认不显示
	 ** @retval								true 成功
	 ** @retval								false 失败
     **/
	static bool LaunchProcessAsUser(const std::wstring& strCmdLine, const std::wstring& strFileName = L"", bool bShowWnd = false);
	/**
     ** 判断当前进程是否以高完整性级别运行 
	 ** @retval								true 是
	 ** @retval								false 否
     **/
	static bool IsProcessRunAsAdmin();
    /**
     ** 判断进程是否以为系统进程(以SYSTEM用户名运行，则认为是系统进程)
     ** @param[in] procID					进程ID，默认使用当前进程ID
	 ** @param[in] procName					进程名字，默认使用当前进程名字
     ** @retval								true 是
     ** @retval								false 否
     **/
    static bool IsSystemProcess(const DWORD& procID = 0);
	static bool IsSystemProcess(const std::wstring& procName = L"");
	/**
     ** 判断进程是否存在
	 ** @param[in] procID					进程ID
     ** @param[in] strProcName				进程名字
     ** @retval								true 存在
     ** @retval								false 不存在
     **/
	static bool IsProcessExist(const DWORD& procID);
    static bool IsProcessExist(const std::wstring& strProcName);

    /**
     ** 获取当前进程的父进程信息：进程名、进程ID
     ** @param[out] parentProcName			进程名
     ** @param[out] procID					进程ID
     ** @retval								true 成功
     ** @retval								false 失败
     **/
    static bool GetParentProcessName(std::wstring& parentProcName, DWORD* procID = NULL);
	/**
     ** 获取运行进程的用户名，如果需要获取SYSTEM用户名，必须以管理员权限启动
     ** @param[in] procID					进程ID，默认使用当前进程ID
     ** @return								用户名字符串
     **/
    static std::wstring GetProcessUserName(const DWORD& procID = 0);
	/**
     ** 设置权限(目前win7系统不支持权限设置)
     ** @param[in] privilege            权限级别名称，这个名称详情见(winnt.h文件的7812行)
	 ** @param[in] bEnablePrivilege     是否启用特殊权限
	 ** @retval							true 成功
	 ** @retval							false 失败
     **/
	static bool MySetPrivilege(const std::wstring& privilege, const bool bEnablePrivilege);
	/**
     ** 通过进程ID获取进程名
     ** @param[in] dwProcID     进程ID
     ** @return                 进程名
     **/
	static std::wstring GetProcessNameByPid(const DWORD dwProcID);
	/**
     ** 通过进程名获取进程ID，解决Win7下无法获取explorer.exe的ID
     ** @param[in] procName     进程名
     ** @param[in] strFlagStr   路径中的一个标示符，因为Chrome和枫树浏览器的进程都是chrome.exe，所以通过该标示符来区分
     ** @return                 进程ID
     **/
    static DWORD GetProcessIDByName(const std::wstring& procName, const std::wstring& strFlagStr = L"");
};

}}
