#pragma once
#include <string>
#include <vector>

/**
 ** 注册表操作类
 ** designed by eric,2016/11/15
 **/
namespace eric { namespace common {
class RegHelper
{
public:
	RegHelper();
	~RegHelper();

public:
	/**
     ** 设置注册表要操作的节点
     ** @param[in] hKey                 该Key值的可选项有：HKEY_CLASSES_ROOT,HKEY_CURRENT_USER,HKEY_LOCAL_MACHINE,HKEY_USERS等，默认为HKEY_CURRENT_USER
     **/
	void SetPrimaryKey(HKEY hKey);	
	/**
     ** 读取指定key注册表项的值,支持REG_SZ,REG_MULTI_SZ,REG_EXPAND_SZ三种
     ** @param[in]  strSubKey           要读取的注册表项的注册表路径
	 ** @param[in]  strKeyName          要读取的注册表项的名称
	 ** @param[out] strKeyValue         注册表项的值
	 ** @param[out] dwType              注册表项的值的类型REG_SZ,REG_MULTI_SZ,REG_EXPAND_SZ
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	bool ReadRegValue(const std::wstring& strSubKey, const std::wstring& strKeyName, std::wstring& strKeyValue, DWORD& dwType, const DWORD samDesired = 0);
	/**
     ** 读取指定key注册表项的值,支持REG_DWORD一种，请注意
     ** @param[in]  strSubKey           要读取的注册表项的注册表路径
	 ** @param[in]  strKeyName          要读取的注册表项的名称
	 ** @param[out] dwKeyValue          注册表项的值
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	bool ReadRegValue(const std::wstring& strSubKey, const std::wstring& strKeyName, DWORD& dwKeyValue, const DWORD samDesired = 0);
	/**
     ** 修改或写入指定的注册表项的值,支持REG_SZ,REG_MULTI_SZ,REG_EXPAND_SZ三种
     ** @param[in]  strSubKey           要修改或写入的注册表项的注册表路径
	 ** @param[in]  strKeyName          要修改或写入的注册表项的名称
	 ** @param[in]  strKeyValue         注册表项的值
	 ** @param[in]  dwType              注册表项的类型，支持REG_SZ,REG_MULTI_SZ,REG_EXPAND_SZ三种
	 ** @param[in]  samDesired          指定注册表项的操作权限
     ** @retval                         true 成功
     ** @retval                         false 失败 
     **/
    bool WriteRegValue(const std::wstring& strSubKey, const std::wstring& strKeyName, const std::wstring& strKeyValue, const DWORD dwType = REG_SZ, const DWORD samDesired = 0);
	/**
	 ** 修改或写入指定的注册表项的值,支持REG_DWORD一种，请注意
	 ** @param[in]  strSubKey           要修改或写入的注册表项的注册表路径
	 ** @param[in]  strKeyName          要修改或写入的注册表项的名称
	 ** @param[in]  dwKeyValue          注册表项的值
	 ** @param[in]  samDesired          指定注册表项的操作权限
     ** @retval                         true 成功
     ** @retval                         false 失败 
     **/
    bool WriteRegValue(const std::wstring& strSubKey, const std::wstring& strKeyName, const DWORD dwKeyValue, const DWORD samDesired = 0);
	/**
     ** 修改或写入指定的注册表项的值,支持REG_DWORD一种，请注意
     ** @param[in]  strSubKey           要删除的注册表项的注册表路径
	 ** @param[in]  strKeyName          要删除的注册表项的名称
	 ** @param[in]  isTree              要删除的项是否是树节点
	 ** @param[in]  samDesired          指定注册表项的操作权限
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	bool DeleteRegValue(const std::wstring& strSubKey, const std::wstring& strKeyName, const bool isTree = false, const DWORD samDesired = 0);

	/**
     ** 删除注册详表项
     ** @param[in]  strSubKey           要删除的注册表项的注册表路径
	 ** @param[in]  samDesired          指定注册表项的操作权限
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	bool DeleteSubKey(const std::wstring& strSubKey, const DWORD samDesired = 0);

	/**
     ** 判断注册表项或者值是否存在
     ** @param[in]  strSubKey           要判断的注册表项的注册表路径
	 ** @param[in]  isTree              要判断的项是否是树节点
	 ** @param[in]  samDesired          指定注册表项的操作权限
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	bool IsRegItemExist(const std::wstring& strSubKey, const bool isTree = true, const std::wstring& strKeyName = L"", const DWORD samDesired = 0);
	/**
     ** 判断注册表项或者值是否存在(HKLM会查找32bit & 64bit注册表)
     ** @param[in]  strSubKey           要判断的注册表项的注册表路径
	 ** @param[in]  isTree              要判断的项是否是树节点
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	bool IsRegItemExistEx(const std::wstring& strSubKey, const bool isTree = true, const std::wstring& strKeyName = L"");
	/**
     ** 枚举某一子项下所有的子项名和子键名
     ** @param[in]  strKeyName          要枚举的子项名
	 ** @param[in,out]  vecTreeKey      返回要枚举的项下所有的子项名
     ** @param[in,out]  vecSubKey       返回要枚举的项下所有的子键名
     **/
	bool EnumAllKey(const std::wstring& strKeyName,std::vector<std::wstring>& vecTreeKey,std::vector<std::wstring>& vecSubKey);
	 /**
     ** 获取HKEY及subKey对应下的对应子项
     ** @param[in]	hRoot				reg root key
     ** @param[in]	strKeyName			subkey
     ** @param[in]	filter				filter
     ** @param[out]	strValue			找到对应filter的最大项
	 ** @param[in]  is64And32			默认为true,搜64注册表和Wow6432Node
     ** @return							true:success false:fail
     **/
    static bool EnumLargestKey(const HKEY hRoot, const std::wstring szKeyName, const std::wstring filter, std::wstring & strValue, bool is64And32 = true); 
private:
	HKEY m_hPrimaryKey;                 // KEY值：HKEY_CLASSES_ROOT,HKEY_CURRENT_USER,HKEY_LOCAL_MACHINE,HKEY_USERS
};

}}
