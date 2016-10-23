// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// libcommon.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������

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
