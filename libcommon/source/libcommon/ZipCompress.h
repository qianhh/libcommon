#pragma once
#include "Buffer.h"

/**
 ** zipѹ�����ѹ
 ** designed by eric,2016/08/20
 **/
class CZipCompress
{
public:
	//gzip
	static bool GZipCompress(char* pSrc, long lSize, Buffer& bufZip); //ѹ��
	static bool GZipUncompress(char* pSrcZip, long lSize, Buffer& buf); //��ѹ

	//zip
	static bool ZipCompress(char* pSrc, long lSize, Buffer& bufZip); //ѹ��
	static bool ZipUncompress(char* pSrcZip, long lSize, Buffer& buf); //��ѹ
};

