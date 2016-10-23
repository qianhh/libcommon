#pragma once
#include "Buffer.h"

/**
 ** ±àÂë×ª»»
 ** designed by eric,2016/09/06
 **/
namespace eric { namespace common {
class Charset
{
public:
	static void UnicodeToGB2312(LPCWSTR lpText, Buffer& bufOut);
	static void GB2312ToUnicode(LPCSTR lpText, Buffer& bufOut);
	static void UnicodeToUTF8(LPCWSTR lpText, Buffer& bufOut);
	static void UTF8ToUnicode(LPCSTR lpText, Buffer& bufOut);
	static void GB2312ToUTF8(LPCSTR lpText, Buffer& bufOut);
	static void UTF8ToGB2312(LPCSTR lpText, Buffer& bufOut);

	static void GB2312ToUrlGB2312(LPCSTR lpText, Buffer& bufOut);
	static void UrlGB2312ToGB2312(LPCSTR lpText, Buffer& bufOut);
	static void GB2312ToUrlUTF8(LPCSTR lpText, Buffer& bufOut);
	static void UrlUTF8ToGB2312(LPCSTR lpText, Buffer& bufOut);
};

}}
