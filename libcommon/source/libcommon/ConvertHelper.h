#pragma once
#include <string>
#include <sstream>

/**
 ** 类型转换操作
 ** designed by eric,2016/11/04
 **/
namespace eric { namespace common {
class ConvertHelper
{
public:
	/**
     ** 类型转换
     ** @param[in] input					输入
     ** @retval								输出
     **/
	template<class InType, class OutType>
	static void Convert(OutType &output, const InType &input)
	{
		std::stringstream ss;
		ss << input;
		ss >> output;
	}
	/**
     ** 类型转换 支持unicode
     ** @param[in] input					输入
     ** @retval								输出
     **/
	template<class InType, class OutType>
	static void ConvertW(OutType &output, const InType &input)
	{
		std::wstringstream ss;
		ss << input;
		ss >> output;
	}
	/**
     ** 类型转换，转成字符串
     ** @param[in] value					输入值
     ** @param[out] out						字符串
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
