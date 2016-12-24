#pragma once
#include <string>
#include <vector>

/**
 ** ע��������
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
     ** ����ע���Ҫ�����Ľڵ�
     ** @param[in] hKey                 ��Keyֵ�Ŀ�ѡ���У�HKEY_CLASSES_ROOT,HKEY_CURRENT_USER,HKEY_LOCAL_MACHINE,HKEY_USERS�ȣ�Ĭ��ΪHKEY_CURRENT_USER
     **/
	void SetPrimaryKey(HKEY hKey);	
	/**
     ** ��ȡָ��keyע������ֵ,֧��REG_SZ,REG_MULTI_SZ,REG_EXPAND_SZ����
     ** @param[in]  strSubKey           Ҫ��ȡ��ע������ע���·��
	 ** @param[in]  strKeyName          Ҫ��ȡ��ע����������
	 ** @param[out] strKeyValue         ע������ֵ
	 ** @param[out] dwType              ע������ֵ������REG_SZ,REG_MULTI_SZ,REG_EXPAND_SZ
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	bool ReadRegValue(const std::wstring& strSubKey, const std::wstring& strKeyName, std::wstring& strKeyValue, DWORD& dwType, const DWORD samDesired = 0);
	/**
     ** ��ȡָ��keyע������ֵ,֧��REG_DWORDһ�֣���ע��
     ** @param[in]  strSubKey           Ҫ��ȡ��ע������ע���·��
	 ** @param[in]  strKeyName          Ҫ��ȡ��ע����������
	 ** @param[out] dwKeyValue          ע������ֵ
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	bool ReadRegValue(const std::wstring& strSubKey, const std::wstring& strKeyName, DWORD& dwKeyValue, const DWORD samDesired = 0);
	/**
     ** �޸Ļ�д��ָ����ע������ֵ,֧��REG_SZ,REG_MULTI_SZ,REG_EXPAND_SZ����
     ** @param[in]  strSubKey           Ҫ�޸Ļ�д���ע������ע���·��
	 ** @param[in]  strKeyName          Ҫ�޸Ļ�д���ע����������
	 ** @param[in]  strKeyValue         ע������ֵ
	 ** @param[in]  dwType              ע���������ͣ�֧��REG_SZ,REG_MULTI_SZ,REG_EXPAND_SZ����
	 ** @param[in]  samDesired          ָ��ע�����Ĳ���Ȩ��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ�� 
     **/
    bool WriteRegValue(const std::wstring& strSubKey, const std::wstring& strKeyName, const std::wstring& strKeyValue, const DWORD dwType = REG_SZ, const DWORD samDesired = 0);
	/**
	 ** �޸Ļ�д��ָ����ע������ֵ,֧��REG_DWORDһ�֣���ע��
	 ** @param[in]  strSubKey           Ҫ�޸Ļ�д���ע������ע���·��
	 ** @param[in]  strKeyName          Ҫ�޸Ļ�д���ע����������
	 ** @param[in]  dwKeyValue          ע������ֵ
	 ** @param[in]  samDesired          ָ��ע�����Ĳ���Ȩ��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ�� 
     **/
    bool WriteRegValue(const std::wstring& strSubKey, const std::wstring& strKeyName, const DWORD dwKeyValue, const DWORD samDesired = 0);
	/**
     ** �޸Ļ�д��ָ����ע������ֵ,֧��REG_DWORDһ�֣���ע��
     ** @param[in]  strSubKey           Ҫɾ����ע������ע���·��
	 ** @param[in]  strKeyName          Ҫɾ����ע����������
	 ** @param[in]  isTree              Ҫɾ�������Ƿ������ڵ�
	 ** @param[in]  samDesired          ָ��ע�����Ĳ���Ȩ��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	bool DeleteRegValue(const std::wstring& strSubKey, const std::wstring& strKeyName, const bool isTree = false, const DWORD samDesired = 0);

	/**
     ** ɾ��ע�������
     ** @param[in]  strSubKey           Ҫɾ����ע������ע���·��
	 ** @param[in]  samDesired          ָ��ע�����Ĳ���Ȩ��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	bool DeleteSubKey(const std::wstring& strSubKey, const DWORD samDesired = 0);

	/**
     ** �ж�ע��������ֵ�Ƿ����
     ** @param[in]  strSubKey           Ҫ�жϵ�ע������ע���·��
	 ** @param[in]  isTree              Ҫ�жϵ����Ƿ������ڵ�
	 ** @param[in]  samDesired          ָ��ע�����Ĳ���Ȩ��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	bool IsRegItemExist(const std::wstring& strSubKey, const bool isTree = true, const std::wstring& strKeyName = L"", const DWORD samDesired = 0);
	/**
     ** �ж�ע��������ֵ�Ƿ����(HKLM�����32bit & 64bitע���)
     ** @param[in]  strSubKey           Ҫ�жϵ�ע������ע���·��
	 ** @param[in]  isTree              Ҫ�жϵ����Ƿ������ڵ�
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	bool IsRegItemExistEx(const std::wstring& strSubKey, const bool isTree = true, const std::wstring& strKeyName = L"");
	/**
     ** ö��ĳһ���������е����������Ӽ���
     ** @param[in]  strKeyName          Ҫö�ٵ�������
	 ** @param[in,out]  vecTreeKey      ����Ҫö�ٵ��������е�������
     ** @param[in,out]  vecSubKey       ����Ҫö�ٵ��������е��Ӽ���
     **/
	bool EnumAllKey(const std::wstring& strKeyName,std::vector<std::wstring>& vecTreeKey,std::vector<std::wstring>& vecSubKey);
	 /**
     ** ��ȡHKEY��subKey��Ӧ�µĶ�Ӧ����
     ** @param[in]	hRoot				reg root key
     ** @param[in]	strKeyName			subkey
     ** @param[in]	filter				filter
     ** @param[out]	strValue			�ҵ���Ӧfilter�������
	 ** @param[in]  is64And32			Ĭ��Ϊtrue,��64ע����Wow6432Node
     ** @return							true:success false:fail
     **/
    static bool EnumLargestKey(const HKEY hRoot, const std::wstring szKeyName, const std::wstring filter, std::wstring & strValue, bool is64And32 = true); 
private:
	HKEY m_hPrimaryKey;                 // KEYֵ��HKEY_CLASSES_ROOT,HKEY_CURRENT_USER,HKEY_LOCAL_MACHINE,HKEY_USERS
};

}}
