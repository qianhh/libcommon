#pragma once
#include <string>
#include <vector>

/**
 ** 字符串操作
 ** designed by eric,2016/11/04
 **/
namespace eric { namespace common {
class StringHelper
{
public:
    /**
     ** 提取小括号中的字符串
     ** @param[in] input					输入
     ** @retval								小括号中的字符串
     **/
	static std::string ExtractInParenthesis(const std::string &input);
	/**
     ** 提取小括号中的字符串
     ** @param[in] input					输入
     ** @param[out] output					小括号中的字符串数组
     **/
	static void ExtractInParenthesis(const std::string &input, std::vector<std::string> &output);
	/**
     ** 分割字符串
     ** @param[in] input					输入
	 ** @param[in] cMask					分隔符
     ** @param[out] output					分割后的字符串数组
     **/
	static void SplitString(const std::string &input, char cMask, std::vector<std::string> &output);
	static void SplitString(const std::wstring &input, wchar_t cMask, std::vector<std::wstring> &output);
	/**
     ** 去掉字符串左边的字符sp，wstr既是输入值，也是输出值
     ** @param[in]   wstr        需要去掉字符sp的字符串
     ** @param[out]  wstr        去除字符sp成功后的字符串
     ** @retval                  true 成功
     ** @retval                  false 失败
     **/
	static bool LTrim(std::wstring& wstr, wchar_t sp = _SPACE);
	/**
     ** 去掉字符串右边的字符sp，wstr既是输入值，也是输出值
     ** @param[in]   wstr        需要去掉字符sp的字符串
     ** @param[out]  wstr        去除字符sp成功后的字符串
     ** @retval                  true 成功
     ** @retval                  false 失败
     **/
    static bool RTrim(std::wstring& wstr, wchar_t sp = _SPACE);
    /**
     ** 去掉字符串左右两边的字符sp，wstr既是输入值，也是输出值
     ** @param[in]   wstr        需要去掉字符sp的字符串
     ** @param[out]  wstr        去除字符sp成功后的字符串
     ** @retval                  true 成功
     ** @retval                  false 失败
     **/
	static bool Trim(std::wstring& wstr, wchar_t sp = _SPACE);
	/**
     ** 字符串替换，将字符串strInput中的strSrc替换成strDst
     ** @param[in]   strInput   原字符串
     ** @param[out]  strInput   替换后的字符串
	 ** @param[in]   strOld     待替换的字符串	
	 ** @param[in]   strNew		目标字符串
     ** @return                  
     **/
	static void Replace(std::wstring &strInput, const std::wstring &strOld, const std::wstring &strNew);
};

}}
