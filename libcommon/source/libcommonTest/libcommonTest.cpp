// libcommonTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "../../lib/libcommon.h"
using namespace eric::common;
//#ifdef _DEBUG
//#pragma comment(lib,"../../lib/Debug/libcommon.lib")
//#else
//#pragma comment(lib,"../../lib/Release/libcommon.lib")
//#endif

#define _CRTDBG_MAP_ALLOC 
//#define new new( _CLIENT_BLOCK, __FILE__, __LINE__) //��ŵ����ɵ�cpp��
int _tmain(int argc, _TCHAR* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(228);
#endif
	//ServerThread server;
	//server.SetAddressPort(11003);
	//server.Initialize();
	//while (1) Sleep(1000*3);
	//server.Exit();

	Logger::GetInstance()->SetMod(Logger::PERDAY_FILE, false);
/*	ClientThread client;
	client.SetAddressIp("127.0.0.1");
	client.SetAddressPort(11000);
	client.Initialize();
	client.Connect();
	Sleep(3000);
	client.Disconnect();
	Sleep(1000);
	client.Connect();

	Json::Value stSend;
	stSend["devid"] = 5001;;
	stSend["behavior"] = "read";
	stSend["devname"]=0;
	Json::FastWriter writer;
	std::string strSend = writer.write(stSend);
	Buffer buf;
	const long DATA_SIZE = strSend.size()+19+1;
	char *pszBuf = (char *)buf.GetBuffer(DATA_SIZE);
	sprintf_s(pszBuf, DATA_SIZE, "%06X%06X%C%06X%s", strSend.size()+19, 1, 'C', strSend.size(), strSend.c_str());
	buf.ReleaseBufferSetSize(DATA_SIZE);
	client.Send(buf);


	while (1)
		Sleep(500);
	client.Exit();
*/
	//NamedPipeServer server;
	//server.Initialize();
	//while (1)
	//	Sleep(500);
	//server.Exit();
	std::vector<std::string> vecResults;
	RegexHelper::Match("(?<=\\().*(?=\\))","i am a script (Script) script file",vecResults, false);

	StringHelper::SplitString("rfd,gg,12g3,,", ',', vecResults);

	std::string dd("xxx");
	ConvertHelper::ToString(dd, 200);
	int a;
	ConvertHelper::ConvertW(a, L"200");

	NamedPipeClient client;
	client.Initialize(L"\\\\.\\pipe\\HardwareService");
	client.WaitPipe();
	client.OpenPipe();

	Json::Value stSend;
	stSend["id"] = 5001;;
	stSend["behavior"] = "read";
	stSend["devname"]=0;
	Json::FastWriter writer;
	std::string strSend = writer.write(stSend);
	Buffer buf;
	const long DATA_SIZE = (long)strSend.size()+19+1;
	char *pszBuf = (char *)buf.GetBuffer(DATA_SIZE);
	sprintf_s(pszBuf, DATA_SIZE, "%06X%06X%C%06X%s", strSend.size()+19, 1, 'C', strSend.size(), strSend.c_str());
	buf.ReleaseBufferSetSize(DATA_SIZE);
	client.Send(buf);
	
//	while (1)
		Sleep(1000);
	client.ClosePipe();

	return 0;
}

