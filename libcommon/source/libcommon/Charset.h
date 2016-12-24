#pragma once
#include "Buffer.h"

/**
 ** ±àÂë×ª»»
 ** designed by eric,2016/09/06
 ** modified by eric,2016/11/04
 **/
namespace eric { namespace common {
class Charset
{
public:
	static void UnicodeToGB2312(LPCWSTR lpText, Buffer& bufOut);
	static std::string UnicodeToGB2312(const std::wstring &unicode);
	static void GB2312ToUnicode(LPCSTR lpText, Buffer& bufOut);
	static std::wstring GB2312ToUnicode(const std::string &gb2312);

	static void UnicodeToUTF8(LPCWSTR lpText, Buffer& bufOut);
	static std::string UnicodeToUTF8(const std::wstring &unicode);
	static void UTF8ToUnicode(LPCSTR lpText, Buffer& bufOut);
	static std::wstring UTF8ToUnicode(const std::string &utf8);

	static void GB2312ToUTF8(LPCSTR lpText, Buffer& bufOut);
	static std::string GB2312ToUTF8(const std::string &gb2312);
	static void UTF8ToGB2312(LPCSTR lpText, Buffer& bufOut);
	static std::string UTF8ToGB2312(const std::string &utf8);


	static void GB2312ToUrlGB2312(LPCSTR lpText, Buffer& bufOut);
	static std::string GB2312ToUrlGB2312(const std::string &gb2312);
	static void UrlGB2312ToGB2312(LPCSTR lpText, Buffer& bufOut);
	static std::string UrlGB2312ToGB2312(const std::string &url_gb2312);

	static void GB2312ToUrlUTF8(LPCSTR lpText, Buffer& bufOut);
	static std::string GB2312ToUrlUTF8(const std::string &gb2312);
	static void UrlUTF8ToGB2312(LPCSTR lpText, Buffer& bufOut);
	static std::string UrlUTF8ToGB2312(const std::string &url_utf8);
};

}}
