#pragma once

#define SD_LOW	L"S:(ML;;NW;;;LW)"
/**
 ** ����ϵͳ�����
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
     ** �ж�ָ���Ľ����Ƿ���WOW64(32λ)��ʽ���У�64λϵͳ��Ч
     ** @param[in] hProc    ���̵ľ��
     ** @retval             true ��
     ** @retval             false ����
     **/
	static bool IsWow64(HANDLE hProc = NULL);
    /**
     ** �ж�ָ���Ľ����Ƿ���64λ��ʽ���У�64λϵͳ��Ч
     ** @param[in] hProc    ���̵ľ��
     ** @retval             true ��
     ** @retval             false ����
     **/
	static bool Is64Proc(HANDLE hProc = NULL);
    /**
     ** ��ȡ��ǰWindowsϵͳ�İ汾-WindowsVersion
     ** @return             Windows�İ汾�ţ�Win8��Win7��Vista��XP
     **/
	static WindowsVersion GetWindowsVersion();
    /**
     ** ��ȡϵͳ����ϸ��Ϣ
     ** @return             ϵͳ����ϸ��Ϣ
     **/
	static std::wstring GetOSDetailInfo();
    /**
     ** ��ǰWindows����ϵͳ�Ƿ���Vista���ϵİ汾
     ** @retval             true ��
     ** @retval             false ����
     **/
	static bool IsVistaOrHigher();
    /**
     ** ������������ͬȨ�޵Ľ����з�����Ϣ����֧��Vista����߰汾
     ** @param[in] msgID            ��ϢID
     **/
	static void SendMsgOnVistaOrHigher(UINT msgID);
	/**
     ** ��ȡ���̿ռ����Ϣ
     ** @param[in]  disk            ���̺ţ��������ð�ţ����磺C:��D:��...
     ** @return                     �����������ַ�������ȷ��С�������λ�����磺15.44
     **/
    static std::wstring GetDiskInfo(const std::wstring& disk = L"C:");
    /**
     ** ��ȡ�����Լ���ֻ֧��Vista�����ϰ汾��0-������Ȩ�ޡ�1-��Ȩ�ޡ�2-�û�Ȩ�ޡ�3-����ԱȨ�ޡ�4-ϵͳȨ�ޣ�
     ** @return                     �����Լ���0-4
     **/
    static int GetIntegrityLevel();

	/**
     ** ������ȫ�����������strSDΪ�գ���ʾ�Ե�ǰĬ�ϵ���ͬȨ�ޣ����strSD��Ϊ�գ���ʾ��Ҫת���İ�ȫ�������ַ���
	 ** @param[in/out]  sa			��ȫ����
	 ** @param[in/out]  sd			��ȫ������
	 ** @param[in]		strSD		��ȫ�������ַ�����Ĭ��Ϊ��
     ** @retval                     true - �ɹ�
	 ** @retval                     false - ʧ��
     **/
	static bool CreateSecurityDescriptor(SECURITY_ATTRIBUTES& sa, SECURITY_DESCRIPTOR& sd, const std::wstring& strSD = L"");
};

}}
