// stdafx.cpp : 只包括标准包含文件的源文件
// libcommon.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

namespace eric { namespace common {
void GetLastErrorMessage(std::string &strMessage, unsigned long dwErrorCode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,0,  
		dwErrorCode ,
		MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,0,NULL);
	strMessage = (char *)lpMsgBuf;
	LocalFree(lpMsgBuf);
}

std::string GetLastErrorMessage(unsigned long dwErrorCode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,0,  
		dwErrorCode ,
		MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,0,NULL);
	std::string strMessage = (char *)lpMsgBuf;
	LocalFree(lpMsgBuf);
	return strMessage;
}

}}
