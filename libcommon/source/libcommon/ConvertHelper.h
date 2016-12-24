#pragma once
#include <string>
#include <sstream>

/**
 ** ����ת������
 ** designed by eric,2016/11/04
 **/
namespace eric { namespace common {
class ConvertHelper
{
public:
	/**
     ** ����ת��
     ** @param[in] input					����
     ** @retval								���
     **/
	template<class InType, class OutType>
	static void Convert(OutType &output, const InType &input)
	{
		std::stringstream ss;
		ss << input;
		ss >> output;
	}
	/**
     ** ����ת�� ֧��unicode
     ** @param[in] input					����
     ** @retval								���
     **/
	template<class InType, class OutType>
	static void ConvertW(OutType &output, const InType &input)
	{
		std::wstringstream ss;
		ss << input;
		ss >> output;
	}
	/**
     ** ����ת����ת���ַ���
     ** @param[in] value					����ֵ
     ** @param[out] out						�ַ���
     **/
	template<class T>
	static void ToString(std::string &output, const T &value)
	{
		Convert(output, value);
	}

	template<class T>
	static void ToString(std::wstring &output, const T &value)
	{
		ConvertW(output, value);
	}
};

}}
