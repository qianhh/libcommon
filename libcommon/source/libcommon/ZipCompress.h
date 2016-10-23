#pragma once
#include "Buffer.h"

/**
 ** zip—πÀı”ÎΩ‚—π
 ** designed by eric,2016/08/20
 **/
class CZipCompress
{
public:
	//gzip
	static bool GZipCompress(char* pSrc, long lSize, Buffer& bufZip); //—πÀı
	static bool GZipUncompress(char* pSrcZip, long lSize, Buffer& buf); //Ω‚—π

	//zip
	static bool ZipCompress(char* pSrc, long lSize, Buffer& bufZip); //—πÀı
	static bool ZipUncompress(char* pSrcZip, long lSize, Buffer& buf); //Ω‚—π
};

