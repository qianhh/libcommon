#pragma once
#include <string>
#include <vector>

/**
 ** �ַ�������
 ** designed by eric,2016/11/04
 **/
namespace eric { namespace common {
class StringHelper
{
public:
    /**
     ** ��ȡС�����е��ַ���
     ** @param[in] input					����
     ** @retval								С�����е��ַ���
     **/
	static std::string ExtractInParenthesis(const std::string &input);
	/**
     ** ��ȡС�����е��ַ���
     ** @param[in] input					����
     ** @param[out] output					С�����е��ַ�������
     **/
	static void ExtractInParenthesis(const std::string &input, std::vector<std::string> &output);
	/**
     ** �ָ��ַ���
     ** @param[in] input					����
	 ** @param[in] cMask					�ָ���
     ** @param[out] output					�ָ����ַ�������
     **/
	static void SplitString(const std::string &input, char cMask, std::vector<std::string> &output);
	static void SplitString(const std::wstring &input, wchar_t cMask, std::vector<std::wstring> &output);
	/**
     ** ȥ���ַ�����ߵ��ַ�sp��wstr��������ֵ��Ҳ�����ֵ
     ** @param[in]   wstr        ��Ҫȥ���ַ�sp���ַ���
     ** @param[out]  wstr        ȥ���ַ�sp�ɹ�����ַ���
     ** @retval                  true �ɹ�
     ** @retval                  false ʧ��
     **/
	static bool LTrim(std::wstring& wstr, wchar_t sp = _SPACE);
	/**
     ** ȥ���ַ����ұߵ��ַ�sp��wstr��������ֵ��Ҳ�����ֵ
     ** @param[in]   wstr        ��Ҫȥ���ַ�sp���ַ���
     ** @param[out]  wstr        ȥ���ַ�sp�ɹ�����ַ���
     ** @retval                  true �ɹ�
     ** @retval                  false ʧ��
     **/
    static bool RTrim(std::wstring& wstr, wchar_t sp = _SPACE);
    /**
     ** ȥ���ַ����������ߵ��ַ�sp��wstr��������ֵ��Ҳ�����ֵ
     ** @param[in]   wstr        ��Ҫȥ���ַ�sp���ַ���
     ** @param[out]  wstr        ȥ���ַ�sp�ɹ�����ַ���
     ** @retval                  true �ɹ�
     ** @retval                  false ʧ��
     **/
	static bool Trim(std::wstring& wstr, wchar_t sp = _SPACE);
	/**
     ** �ַ����滻�����ַ���strInput�е�strSrc�滻��strDst
     ** @param[in]   strInput   ԭ�ַ���
     ** @param[out]  strInput   �滻����ַ���
	 ** @param[in]   strOld     ���滻���ַ���	
	 ** @param[in]   strNew		Ŀ���ַ���
     ** @return                  
     **/
	static void Replace(std::wstring &strInput, const std::wstring &strOld, const std::wstring &strNew);
};

}}
