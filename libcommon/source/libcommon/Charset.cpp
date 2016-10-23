#include "StdAfx.h"
#include "Charset.h"

void Charset::UnicodeToGB2312(LPCWSTR lpText, Buffer& bufOut)
{
	bufOut.Empty();
	int nDestLength=0;
	if (lpText) nDestLength=::WideCharToMultiByte(936, 0, lpText, -1, NULL, 0, NULL, NULL)-1;
	if (nDestLength>0)
	{
		LPSTR lpDest=(LPSTR)bufOut.GetBuffer(nDestLength+10);
		::WideCharToMultiByte(936, 0, lpText, -1, lpDest, nDestLength, NULL, NULL);
		bufOut.ReleaseBufferSetSize(nDestLength);
	}
}

void Charset::GB2312ToUnicode(LPCSTR lpText, Buffer& bufOut)
{
	bufOut.Empty();
	int nDestLength=0;

	if (lpText) nDestLength=::MultiByteToWideChar(936, 0, lpText, -1, NULL, 0)-1;
	if (nDestLength>0)
	{
		LPWSTR lpDest=(LPWSTR)bufOut.GetBuffer((nDestLength+10)*sizeof(wchar_t));
		::MultiByteToWideChar(936, 0, lpText, -1, lpDest, nDestLength);
		bufOut.ReleaseBufferSetSize(nDestLength*sizeof(wchar_t));
	}
}

/*
UNICODE UTF-8编码表
U-00000000 - U-0000007F: 0xxxxxxx 
U-00000080 - U-000007FF: 110xxxxx 10xxxxxx 
U-00000800 - U-0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx 
U-00010000 - U-001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx 
U-00200000 - U-03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 
U-04000000 - U-7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
void Charset::UnicodeToUTF8(LPCWSTR lpText, Buffer& bufOut)
{
	bufOut.Empty();
	if (!lpText) return;

	long lBufLength = 0;
	wchar_t wChar = 0;
	LPWSTR lpCurrent = (LPWSTR)lpText;
	for (; wChar = *lpCurrent; ++lpCurrent) //计算转换后的长度
	{
		if (wChar < 0x80) ++lBufLength;
		else if (wChar < 0x800)
			lBufLength += 2;
		else if (wChar < 0x10000)
			lBufLength += 3;
		else if (wChar < 0x200000)
			lBufLength += 4;
		else if (wChar < 0x4000000)
			lBufLength += 5;
		else
			lBufLength += 6;
	}

	if (lBufLength <= 0) return;
	char *pBuffer = (char *)bufOut.GetBuffer(lBufLength + 1);
	if (!pBuffer) return;
	lpCurrent = (LPWSTR)lpText;
	for (; wChar = *lpCurrent; ++lpCurrent) //计算转换后的长度
	{
		if (wChar < 0x80)
		{
			*pBuffer = (char)wChar;
			++pBuffer;
		}
		else if (wChar < 0x800)
		{
			*pBuffer = 0xc0 | (wChar >> 6);
			*(pBuffer+1) = 0x80 | (wChar & 0x3f);
			pBuffer += 2;
		}
		else if (wChar < 0x10000)
		{
			*pBuffer = 0xe0 | (wChar >> 12);
			*(pBuffer+1) = 0x80 | ((wChar >> 6) & 0x3f);
			*(pBuffer+2) = 0x80 | (wChar & 0x3f);
			pBuffer += 3;
		}
		else if (wChar < 0x200000)
		{
			*pBuffer = 0xf0 | ((int)wChar >> 18);
			*(pBuffer+1) = 0x80 | ((wChar >> 12) & 0x3f);
			*(pBuffer+2) = 0x80 | ((wChar >> 6) & 0x3f);
			*(pBuffer+3) = 0x80 | (wChar & 0x3f);
			pBuffer += 4;
		}
		else if (wChar < 0x4000000)
		{
			*pBuffer = 0xf8 | ((int)wChar >> 24);
			*(pBuffer+1) = 0x80 | (((int)wChar >> 18) & 0x3f);
			*(pBuffer+2) = 0x80 | ((wChar >> 12) & 0x3f);
			*(pBuffer+3) = 0x80 | ((wChar >> 6) & 0x3f);
			*(pBuffer+4) = 0x80 | (wChar & 0x3f);
			pBuffer += 5;
		}
		else
		{
			*pBuffer = 0xfc | ((int)wChar >> 30);
			*(pBuffer+1) = 0x80 | (((int)wChar >> 24) & 0x3f);
			*(pBuffer+2) = 0x80 | (((int)wChar >> 18) & 0x3f);
			*(pBuffer+3) = 0x80 | ((wChar >> 12) & 0x3f);
			*(pBuffer+4) = 0x80 | ((wChar >> 6) & 0x3f);
			*(pBuffer+5) = 0x80 | (wChar & 0x3f);
			pBuffer += 6;
		}
	}
	bufOut.ReleaseBufferSetSize(lBufLength);
}

void Charset::UTF8ToUnicode(LPCSTR lpText, Buffer& bufOut)
{
	bufOut.Empty();
	if (!lpText) return;

	unsigned char chUTF8 = 0;
	long lBufLength = 0;
	long i = 0;
	long lUTF8CharSize = 0; //一个UTF8字符所占字节
	LPSTR lpCurrent = (LPSTR)lpText;
	while (chUTF8 = *lpCurrent) //计算转换后的长度
	{
		if ((chUTF8 & 0x80) == 0)
			lUTF8CharSize = 1;
		else if ((chUTF8 & 0xe0) == 0xc0)
			lUTF8CharSize = 2;
		else if ((chUTF8 & 0xf0) == 0xe0)
			lUTF8CharSize = 3;
		else if ((chUTF8 & 0xf8) == 0xf0)
			lUTF8CharSize = 4;
		else if ((chUTF8 & 0xfc) == 0xf8)
			lUTF8CharSize = 5;
		else if ((chUTF8 & 0xfe) == 0xfc)
			lUTF8CharSize = 6;
		else
			break;
		for (i = 0; i < lUTF8CharSize-1; ++i)
			if (!*(++lpCurrent)) break; //UTF8字码不足
		if (i < lUTF8CharSize-1) break;
		++lBufLength;
		++lpCurrent;
	}

	if (lBufLength <= 0) return;
	LPWSTR lpDest = (LPWSTR)bufOut.GetBuffer((lBufLength+1)*sizeof(wchar_t));
	if (!lpDest) return;
	long lDestIndex = 0;
	lpCurrent = (LPSTR)lpText;
	while ((chUTF8 = *lpCurrent) && lDestIndex < lBufLength)
	{
		wchar_t& wChar = lpDest[lDestIndex];
		if ((chUTF8 & 0x80) == 0)
		{
			lUTF8CharSize = 1;
			wChar = chUTF8;
		}
		else if ((chUTF8 & 0xe0) == 0xc0)
		{
			lUTF8CharSize = 2;
			wChar = (chUTF8 & 0x3f) << 6;
			wChar |= ((lpCurrent[1]) & 0x3f);
		}
		else if ((chUTF8 & 0xf0) == 0xe0)
		{
			lUTF8CharSize = 3;
			wChar = (chUTF8 & 0x1f) << 12;
			wChar |= (lpCurrent[1] & 0x3f) << 6;
			wChar |= ((lpCurrent[2]) & 0x3f);
		}
		else if ((chUTF8 & 0xf8) == 0xf0)
		{
			lUTF8CharSize = 4;
			wChar = (chUTF8 & 0x0f) << 18;
			wChar |= (lpCurrent[1] & 0x3f) << 12;
			wChar |= (lpCurrent[2] & 0x3f) << 6;
			wChar |= ((lpCurrent[3]) & 0x3f);
		}
		else if ((chUTF8 & 0xfc) == 0xf8)
		{
			lUTF8CharSize = 5;
			wChar = (chUTF8 & 0x07) << 24;
			wChar |= (lpCurrent[1] & 0x3f) << 18;
			wChar |= (lpCurrent[2] & 0x3f) << 12;
			wChar |= (lpCurrent[3] & 0x3f) << 6;
			wChar |= ((lpCurrent[4]) & 0x3f);
		}
		else if ((chUTF8 & 0xfe) == 0xfc)
		{
			lUTF8CharSize = 6;
			wChar = (chUTF8 & 0x03) << 30;
			wChar |= (lpCurrent[1] & 0x3f) << 24;
			wChar |= (lpCurrent[2] & 0x3f) << 18;
			wChar |= (lpCurrent[3] & 0x3f) << 12;
			wChar |= (lpCurrent[4] & 0x3f) << 6;
			wChar |= ((lpCurrent[5]) & 0x3f);
		}
		
		++lDestIndex;
		lpCurrent += lUTF8CharSize;
	}
	lpDest[lBufLength] = 0;
	bufOut.ReleaseBufferSetSize(lBufLength*sizeof(wchar_t));
}

void Charset::GB2312ToUTF8(LPCSTR lpText, Buffer& bufOut)
{
	Buffer buf;
	GB2312ToUnicode(lpText, buf);
	UnicodeToUTF8((LPCWSTR)buf.GetBuffer(), bufOut);
}

void Charset::UTF8ToGB2312(LPCSTR lpText, Buffer& bufOut)
{
	Buffer buf;
	UTF8ToUnicode(lpText, buf);
	UnicodeToGB2312((LPCWSTR)buf.GetBuffer(), bufOut);
}

//把lpText编码为网页中的 GB2312 url encode ,英文不变，汉字双字节  如%3D%AE
void Charset::GB2312ToUrlGB2312(LPCSTR lpText, Buffer& bufOut)
{
	bufOut.Empty();
	if (!lpText) return;
	char tempbuff[4] = {0};
	long lLength = (long)strlen(lpText);
    for (long i = 0; i < lLength; ++i)
    {
        if (isalnum((BYTE)lpText[i]))
			bufOut.Append(lpText+i, 1);
        else if (isspace((BYTE)lpText[i]))
            bufOut.Append("+", 1);
        else
        {
            sprintf_s(tempbuff, "%%%X%X", (BYTE)(lpText[i]) >> 4, lpText[i] & 0x0f);
            bufOut.Append(tempbuff, 3);
        }
    }
}

//把lpText编码为网页中的 UTF-8 url encode ,英文不变，汉字三字节  如%3D%AE%88
void Charset::GB2312ToUrlUTF8(LPCSTR lpText, Buffer& bufOut)
{
	Buffer buf;
    GB2312ToUTF8(lpText, buf);
	GB2312ToUrlGB2312((LPCSTR)buf.GetBuffer(), bufOut);
}

//做为解Url使用
char CharToInt(char ch)
{
	if(ch>='0' && ch<='9')return (char)(ch-'0');
	if(ch>='a' && ch<='f')return (char)(ch-'a'+10);
	if(ch>='A' && ch<='F')return (char)(ch-'A'+10);
	return -1;
}

char HexStrToBin(LPCSTR str)
{
	char tempWord[2] = {0};
	tempWord[0] = CharToInt(str[0]);		//make the B to 11 -- 00001011
	tempWord[1] = CharToInt(str[1]);		//make the 0 to 0  -- 00000000
	return (tempWord[0] << 4) | tempWord[1];//to change the BO to 10110000
}

void Charset::UrlGB2312ToGB2312(LPCSTR lpText, Buffer& bufOut)
{
	bufOut.Empty();
	if (!lpText) return;

	char tmp[2] = {0};
	char dest = 0;
	long i = 0, lLength = (long)strlen(lpText);
	while (i < lLength)
	{
		if (lpText[i] == '%')
		{
			tmp[0] = lpText[i+1];
			tmp[1] = lpText[i+2];
			dest = HexStrToBin(tmp);
			i += 3;
		}
		else if (lpText[i] == '+')
		{
			dest = ' ';
			++i;
		}
		else
		{
			dest = lpText[i];
			++i;
		}
		bufOut.Append(&dest, 1);
	}
	
	dest = '\0';
	bufOut.Append(&dest,1);
}

void Charset::UrlUTF8ToGB2312(LPCSTR lpText, Buffer& bufOut)
{
	Buffer buf;
	UrlGB2312ToGB2312(lpText, buf);
	UTF8ToGB2312((LPCSTR)buf.GetBuffer(), bufOut);
}
