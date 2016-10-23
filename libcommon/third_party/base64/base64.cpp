#include "base64.h"

//base64编码
std::string Encode64(const unsigned char* Data,int DataByte)
{
	//编码表
	const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	int lOutMax = DataByte*4/3+10;
	std::string strEncode;
	strEncode.reserve(lOutMax);
	strEncode.resize(lOutMax);
	char* pcEncode = (char *)strEncode.c_str();
	int lOutLen = 0;
	unsigned char Tmp[4]={0};
	for(int i=0;i<(int)(DataByte / 3);i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		pcEncode[lOutLen++] = EncodeTable[Tmp[1] >> 2];
		pcEncode[lOutLen++] = EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		pcEncode[lOutLen++] = EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		pcEncode[lOutLen++] = EncodeTable[Tmp[3] & 0x3F];
	}
	//对剩余数据进行编码
	int Mod=DataByte % 3;
	if(Mod==1)
	{
		Tmp[1] = *Data++;
		pcEncode[lOutLen++] = EncodeTable[(Tmp[1] & 0xFC) >> 2];
		pcEncode[lOutLen++] = EncodeTable[((Tmp[1] & 0x03) << 4)];
		pcEncode[lOutLen++] = '=';
		pcEncode[lOutLen++] = '=';
	}
	else if(Mod==2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		pcEncode[lOutLen++] = EncodeTable[(Tmp[1] & 0xFC) >> 2];
		pcEncode[lOutLen++] = EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		pcEncode[lOutLen++] = EncodeTable[((Tmp[2] & 0x0F) << 2)];
		pcEncode[lOutLen++] = '=';
	}
	pcEncode[lOutLen++] = '\0';
	strEncode.resize(lOutLen);

	return strEncode;
}

//base64解码
std::string Decode64(const char* Data,int DataByte,int& OutByte)
{
	//解码表
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'
		0, 0, 0,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};
	//返回值
	int lOutMax = DataByte * 3 / 4 + 10;
	std::string strDecode;
	strDecode.reserve(lOutMax);
	strDecode.resize(lOutMax);
	unsigned char* pucDecode = (unsigned char *)strDecode.c_str();
	int lOutLen = 0;
	int nValue;
	int i= 0;
	while (i < DataByte)
	{
		nValue = DecodeTable[*Data++] << 18;
		nValue += DecodeTable[*Data++] << 12;
		pucDecode[lOutLen++] = (nValue & 0x00FF0000) >> 16;
		OutByte++;
		if (*Data != '=')
		{
			nValue += DecodeTable[*Data++] << 6;
			pucDecode[lOutLen++] = (nValue & 0x0000FF00) >> 8;
			OutByte++;
			if (*Data != '=')
			{
				nValue += DecodeTable[*Data++];
				pucDecode[lOutLen++] = nValue & 0x000000FF;
				OutByte++;
			}
		}
		i += 4;
	}
	OutByte = lOutLen;
	strDecode.resize(lOutLen);
	return strDecode;
}
