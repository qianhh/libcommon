#include <Winsock2.h>
#include "ClientThread.h"
#include <process.h>
#include "log.h"
#include <Windows.h>
#include <iphlpapi.h>
#include <string>
#include <sstream>
#include "Buffer.h"
#include "Event.h"
#include "common.h"
#include "hash/md5.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"iphlpapi.lib")
using namespace std;

#ifndef CLOSE_SOCKET
#define CLOSE_SOCKET(socket) if (socket) { closesocket(socket); socket = 0; }
#endif

///////////////////////////////////////////////////////////////////////////////
ClientThread::ClientThread(void) : m_hThread(NULL), m_uThreadId(0), m_nPort(10000)
	, m_sockClient(0), m_bExit(false), m_pEventConnect(new Event(FALSE, TRUE))
	, m_nMaxPort(m_nPort + 50), m_bFloatPort(false)
{
	memset(m_szIp, 0, sizeof(m_szIp));
	
	WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2) , &wsaData) != 0) 
    {
        LOG_TRACE("WSAStartup call failed, error[%d]:%s", WSAGetLastError(), GetLastErrorMessage(WSAGetLastError()).c_str());
    }
}

ClientThread::~ClientThread(void)
{
	Exit();
	DELETE_PTR(m_pEventConnect);
    WSACleanup();
}

void ClientThread::SetAddressIp(const char *pszIp)
{
	strcpy_s(m_szIp, pszIp);
}

void ClientThread::SetAddressPort(unsigned short nPort)
{
	m_nPort = nPort;
	m_nMaxPort = m_nPort + 50;
}

void ClientThread::SetAddress(const char *pszIp, unsigned short nPort)
{
	strcpy_s(m_szIp, pszIp);
	SetAddressPort(nPort);
}

void ClientThread::EnableFloatPort(bool bEnable)
{
	m_bFloatPort = bEnable;
}

bool ClientThread::Initialize(void)
{
	if (m_hThread) return false;
	//create thread
	m_hThread = _beginthreadex(NULL, 0, ClientThread::ThreadFunction, reinterpret_cast<void*>(this), 0, &m_uThreadId);
	if (!m_hThread) 
	{
		return false;
	}
	
	return true;
}

void ClientThread::Exit(void)
{
	m_bExit = true;
	if (m_hThread)
	{
		if (WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hThread, 500))
			TerminateThread((HANDLE)m_hThread, 1);
		CLOSE_HANDLE(m_hThread);
	}
	CLOSE_SOCKET(m_sockClient);
}

unsigned int WINAPI ClientThread::ThreadFunction(void* pContext)
{
	ClientThread *pThread = static_cast<ClientThread*>(pContext);
	
	DWORD dwResult = 0;
#ifdef _DEBUG
	dwResult = pThread->Execute();
#else
	__try
	{
		dwResult = pThread->Execute();
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#endif
	return dwResult;
}

unsigned int ClientThread::Execute()
{
    while (!m_bExit)
	{
		if (!m_pEventConnect->Lock(500))
		{
			continue;
		}
		long lPackageSize = 0;

		long lRecvSize = recv(m_sockClient, (char *)&lPackageSize, sizeof(lPackageSize), 0);

		if(lRecvSize <= 0) //socket关闭 或 出错
		{
			if (lRecvSize == 0)
				LOG_TRACE("socket[%d] connect is closed", m_sockClient);
			else
				LOG_TRACE("socket[%d] receive failed, error[%d]:%s", m_sockClient, WSAGetLastError(), GetLastErrorMessage(WSAGetLastError()).c_str());
			Disconnect();
			break;
		}
		Buffer buffer;
		char *pBuf = (char *)buffer.GetBufferSetSize(lPackageSize);
		lRecvSize = 0;
		while (lRecvSize < lPackageSize)
		{
			long lRecv = recv(m_sockClient, pBuf+lRecvSize, lPackageSize-lRecvSize, 0);
			if(lRecv <= 0) //socket关闭 或 出错
			{
				if (lRecvSize == 0)
					LOG_TRACE("socket[%d] connect is closed", m_sockClient);
				else
					LOG_TRACE("socket[%d] receive failed, error[%d]:%s", m_sockClient, WSAGetLastError(), GetLastErrorMessage(WSAGetLastError()).c_str());
				Disconnect();
				break;
			}
			lRecvSize += lRecv;
		}

		LOG_TRACE("socket[%d] received buffer of %d size", m_sockClient, buffer.GetSize());
		OnReadEvent(buffer);
		LOG_TRACE("finished, sock=%d", m_sockClient);
	}

	return 0;
}

void ClientThread::OnReadEvent(const Buffer &bufRecv)
{
}

bool ClientThread::Send(const Buffer &bufSend)
{
	if (!m_sockClient)
	{
		LOG_TRACE("send buffer error, don't connect to the server");
		return false;
	}

	long lSize = bufSend.GetSize();

	//先发送长度
	send(m_sockClient, (const char *)&lSize, sizeof(lSize), 0);

	send(m_sockClient, (const char *)bufSend.GetBuffer(), lSize, 0);
	LOG_TRACE("socket[%d] send buffer of %d size", m_sockClient, lSize);
	return true;
}

void ClientThread::Disconnect(void)
{
	LOG_TRACE("close connect, sock=%d", m_sockClient);
	CLOSE_SOCKET(m_sockClient);
	m_pEventConnect->ResetEvent();
}

bool ClientThread::Connect(void)
{
	if (m_sockClient) return true;

	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(m_nPort);
	server.sin_addr.s_addr = inet_addr(m_szIp);
	m_sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//IPPROTO_TCP

	bool bConnected = false;
	while (1)
	{
		LOG_TRACE("try to connect server ip[%s:%d]...", m_szIp, m_nPort);
		server.sin_port = htons(m_nPort);
		if (0 == connect(m_sockClient, (sockaddr*)&server, sizeof(server))) 
		{
			LOG_TRACE("connect success, sock=%d", m_sockClient);
			//进行连接测试
			if (TestConnect())
			{
				//取消超时设置
				struct timeval tv_out;
				tv_out.tv_sec = 0;
				tv_out.tv_usec = 0;
				setsockopt(m_sockClient, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv_out, sizeof(tv_out));

				bConnected = true;
				m_pEventConnect->SetEvent();
				LOG_TRACE("connect correct server, sock=%d", m_sockClient);
				break;
			}
			else
			{
				Disconnect();
				m_sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//IPPROTO_TCP
			}
		}
		LOG_TRACE("connect server ip[%s:%d] failed, error[%d]:%s", m_szIp, m_nPort, WSAGetLastError(), GetLastErrorMessage(WSAGetLastError()).c_str());
		if (!m_bFloatPort) break;

		++m_nPort;
		if (m_nPort > m_nMaxPort)
		{
			m_nPort = m_nMaxPort - 50;
			LOG_TRACE("connect server ip[%s] port[%d-%d] failed, error[%d]:%s", m_szIp, m_nPort, m_nMaxPort, WSAGetLastError()
				, GetLastErrorMessage(WSAGetLastError()).c_str());
			break;
		}
	}
	if (!bConnected) Disconnect();
	return bConnected;
}

bool ClientThread::TestConnect(void)
{
	if (!m_sockClient) return false;
	struct timeval tv_out;
    tv_out.tv_sec = 1; //1s
    tv_out.tv_usec = 0;
	setsockopt(m_sockClient, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv_out, sizeof(tv_out));
	//暗号 月薪上万 工资日结
	Buffer buffer;
	buffer.AppendStringA("月薪上万");
	Send(buffer);
	//接收
	long lPackageSize = 0;
	long lRecvSize = recv(m_sockClient, (char *)&lPackageSize, sizeof(lPackageSize), 0);
	if (lPackageSize != buffer.GetSize()) return false; //校验长度
	if(lRecvSize <= 0) //socket关闭 或 出错
	{
		if (lRecvSize == 0)
			LOG_TRACE("socket[%d] connect is closed", m_sockClient);
		else
			LOG_TRACE("socket[%d] receive failed, error[%d]:%s", m_sockClient, WSAGetLastError(), GetLastErrorMessage(WSAGetLastError()).c_str());
		return false;
	}
	char *pBuf = (char *)buffer.GetBufferSetSize(lPackageSize);
	lRecvSize = 0;
	while (lRecvSize < lPackageSize)
	{
		long lRecv = recv(m_sockClient, pBuf+lRecvSize, lPackageSize-lRecvSize, 0);
		if(lRecv <= 0) //socket关闭 或 出错
		{
			if (lRecvSize == 0)
				LOG_TRACE("socket[%d] connect is closed", m_sockClient);
			else
				LOG_TRACE("socket[%d] receive failed, error[%d]:%s", m_sockClient, WSAGetLastError(), GetLastErrorMessage(WSAGetLastError()).c_str());
			return false;
		}
		lRecvSize += lRecv;
	}
	string strResp((const char *)buffer.GetBuffer(), buffer.GetSize());
	if (strResp == "工资日结") return true;
	return false;
}
