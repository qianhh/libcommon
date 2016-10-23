#include "StdAfx.h"
#include "ZipCompress.h"
#include "Buffer.h"
#include "zlib/inc/zlib.h"

//NOTE: 在预处理器定义中加上ZLIB_WINAPI

namespace eric { namespace common {
// Compress gzip data
// data 原数据 ndata 原数据长度 zdata 压缩后数据 nzdata 压缩后长度
int gzcompress(Bytef *data, uLong ndata, Bytef *zdata, uLong *nzdata)
{
	z_stream c_stream;
	int err = 0;

	if(data && ndata > 0)
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		//只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
		if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;

		c_stream.next_in  = data;
		c_stream.avail_in  = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out  = *nzdata;
		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata) 
		{
			if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
        if(c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if(err != Z_OK) return -1;
		}
		if(deflateEnd(&c_stream) != Z_OK) return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}
 
// Uncompress gzip data
// zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度
int gzdecompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = NULL;
    d_stream.zfree = NULL;
    d_stream.opaque = NULL;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    //只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
    if(inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
    //if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
    while(d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK) {
            if(err == Z_DATA_ERROR) {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
                    return -1;
                }
            } else return -1;
        }
    }
    if(inflateEnd(&d_stream) != Z_OK) return -1;
    *ndata = d_stream.total_out;
    return 0;
}

#define CHUNK 32768
int gzuncompress(Bytef *zdata, uLong nzdata, Buffer& buf)
{
	Buffer Buffer;
	int err = 0;
	z_stream d_stream = {0}; /* decompression stream */
	static char dummy_head[2] = {
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = NULL;
	d_stream.zfree = NULL;
	d_stream.opaque = NULL;
	d_stream.next_in  = zdata;
	d_stream.avail_in = nzdata;
	Bytef *out = (Bytef *)Buffer.GetBuffer(CHUNK);
	uLong have = 0;
	//只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
	if(inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
	//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
	while(d_stream.total_in < nzdata) {
		d_stream.next_out = out;
		d_stream.avail_out = CHUNK; /* force small buffers */
		err = inflate(&d_stream, Z_NO_FLUSH);
		have = CHUNK - d_stream.avail_out;
		buf.Append(out, have);

		if(err != Z_OK) {
			if(err == Z_DATA_ERROR) {
				d_stream.next_in = (Bytef*) dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
					break;
				}
			} else break;
		}
	}
	if(inflateEnd(&d_stream) != Z_OK) return -1;
	return (err==Z_OK||err==Z_STREAM_END) ? 0 : err;
}


bool CZipCompress::GZipCompress(char* pSrc, long lSize, Buffer& bufZip)
{
	bufZip.Empty();
	unsigned long lZipSize=lSize+10;
	char* pZipBuffer=(char*)bufZip.GetBuffer(lZipSize);
	
	int nResult=gzcompress((Bytef*)pSrc,lSize,(Bytef*)pZipBuffer,&lZipSize);
	if (nResult==0) bufZip.ReleaseBufferSetSize(lZipSize);
	return nResult==0;
}

bool CZipCompress::GZipUncompress(char* pSrcZip, long lSize, Buffer& buf)
{
	Buffer buffer;
	int nResult = gzuncompress((Bytef*)pSrcZip, lSize, buffer);
	buf.Append(buffer.GetBuffer(), buffer.GetSize());
	return nResult==0;
}

bool CZipCompress::ZipCompress(char* pSrc, long lSize, Buffer& bufZip)
{
	uLong lDestSize = compressBound((uLong)lSize);
	bufZip.Empty();
	char *pDest = (char *)bufZip.GetBuffer(lDestSize + 4); //头4字节存放原始长度
	*(long *)pDest = lSize;
	int nResult = compress((Bytef*)(pDest + 4), &lDestSize, (Bytef*)pSrc, lSize);
	bufZip.ReleaseBufferSetSize(lDestSize + 4);
	switch (nResult)
	{
	case Z_MEM_ERROR:
	case Z_DATA_ERROR:
	case Z_BUF_ERROR:
		break;
	}
	return nResult==Z_OK;
}

bool CZipCompress::ZipUncompress(char* pSrcZip, long lSize, Buffer& buf)
{
	long lDsstSize = *(long *)pSrcZip;
	int nResult = uncompress((Bytef*)buf.GetBuffer(lDsstSize), (uLongf*)&lDsstSize, (Bytef*)(pSrcZip + 4), lSize - 4);
	buf.ReleaseBufferSetSize(lDsstSize);
	return nResult==Z_OK;
}

}}
