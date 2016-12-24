#pragma once

#define SD_LOW	L"S:(ML;;NW;;;LW)"
/**
 ** 操作系统相关类
 ** designed by eric,2016/11/14
 **/
namespace eric { namespace common {
class OSHelper
{
public:
	enum WindowsVersion
	{
		UnknownVer,
		WinXP,
		WinVista,
		Win7,
		Win8
	};

    /**
     ** 判断指定的进程是否以WOW64(32位)方式运行，64位系统有效
     ** @param[in] hProc    进程的句柄
     ** @retval             true 是
     ** @retval             false 不是
     **/
	static bool IsWow64(HANDLE hProc = NULL);
    /**
     ** 判断指定的进程是否以64位方式运行，64位系统有效
     ** @param[in] hProc    进程的句柄
     ** @retval             true 是
     ** @retval             false 不是
     **/
	static bool Is64Proc(HANDLE hProc = NULL);
    /**
     ** 获取当前Windows系统的版本-WindowsVersion
     ** @return             Windows的版本号：Win8、Win7、Vista、XP
     **/
	static WindowsVersion GetWindowsVersion();
    /**
     ** 获取系统的详细信息
     ** @return             系统的详细信息
     **/
	static std::wstring GetOSDetailInfo();
    /**
     ** 当前Windows操作系统是否是Vista以上的版本
     ** @retval             true 是
     ** @retval             false 不是
     **/
	static bool IsVistaOrHigher();
    /**
     ** 允许在两个不同权限的进程中发送消息，仅支持Vista或更高版本
     ** @param[in] msgID            消息ID
     **/
	static void SendMsgOnVistaOrHigher(UINT msgID);
	/**
     ** 获取磁盘空间的信息
     ** @param[in]  disk            磁盘号，必须带上冒号！比如：C:、D:、...
     ** @return                     磁盘容量的字符串，精确到小数点后两位；比如：15.44
     **/
    static std::wstring GetDiskInfo(const std::wstring& disk = L"C:");
    /**
     ** 获取完整性级别，只支持Vista及以上版本（0-不信任权限、1-低权限、2-用户权限、3-管理员权限、4-系统权限）
     ** @return                     完整性级别，0-4
     **/
    static int GetIntegrityLevel();

	/**
     ** 创建安全描述符，如果strSD为空，表示以当前默认的相同权限；如果strSD不为空，表示需要转换的安全描述符字符串
	 ** @param[in/out]  sa			安全属性
	 ** @param[in/out]  sd			安全描述符
	 ** @param[in]		strSD		安全描述符字符串，默认为空
     ** @retval                     true - 成功
	 ** @retval                     false - 失败
     **/
	static bool CreateSecurityDescriptor(SECURITY_ATTRIBUTES& sa, SECURITY_DESCRIPTOR& sd, const std::wstring& strSD = L"");
};

}}
