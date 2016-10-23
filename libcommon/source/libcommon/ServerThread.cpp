#include <Winsock2.h>
#include "ServerThread.h"
#include <process.h>
#include <event2/event.h>
#include "log.h"
#include <Windows.h>
#include <iphlpapi.h>
#include <string>
#include "ThreadPool.h"
#include "Buffer.h"
#include "common.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"iphlpapi.lib")
using namespace std;

#ifndef CLOSE_HANDLE
#define CLOSE_HANDLE(handle) if (handle) { ::CloseHandle((HANDLE)handle); handle = NULL; }
#endif
#define BACKLOG     10

namespace eric { namespace common {
//用户写事件完成后的销毁，在on_write()中执行
struct sock_ev_write
{
    struct event* write_ev;
    Buffer *buffer;
};

//用于读事件终止（socket断开）后的销毁
struct sock_ev_read
{
	ServerThread *pServer;
    struct event_base* base;//因为socket断掉后，读事件的loop要终止，所以要有base指针
    struct event* read_ev;
};

//accept线程需要传递的参数
struct accept_param
{
	int sockfd;
	ServerThread *pServer;
};

//用于传递客户端socket连接参数
struct socket_param
{
	int socket;
	event_base *base;
};

const static BOOL PortUsedTCP(ULONG uPort)
{
	MIB_TCPTABLE TcpTable[100];
	DWORD nSize = sizeof(TcpTable);
	if(NO_ERROR == GetTcpTable(&TcpTable[0],&nSize,TRUE))
	{
		DWORD nCount = TcpTable[0].dwNumEntries;
		if (nCount > 0)
		{
			for (DWORD i = 0; i < nCount; ++i)
			{
				MIB_TCPROW TcpRow = TcpTable[0].table[i];
				DWORD temp1 = TcpRow.dwLocalPort;
				int temp2 = temp1 / 256 + (temp1 % 256) * 256;
				if(temp2 == uPort)
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	return FALSE;
}

string GetLocalIpAddress()
{
	WORD wVersionRequested = MAKEWORD(2, 2);

	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return "";

	char local[255] = {0};
	gethostname(local, sizeof(local));
	hostent* ph = gethostbyname(local);
	if (ph == NULL)
		return "";

	in_addr addr;
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // 这里仅获取第一个ip

	string localIP;
	localIP.assign(inet_ntoa(addr));

	WSACleanup();
	return localIP;
}

///////////////////////////////////////////////////////////////////////////////
ServerThread::ServerThread(void) : m_hThread(NULL), m_uThreadId(0), m_base(NULL), m_nPort(10000), m_evlisten(NULL)
	, m_listener(0), m_nMaxPort(m_nPort+50), m_bFloatPort(false)
{
}

ServerThread::~ServerThread(void)
{
	CLOSE_HANDLE(m_hThread);
	if (m_base) event_base_free(m_base);
    WSACleanup();
}

void destroy_sock_ev_write(struct sock_ev_write* sock_ev_write_struct)
{
    if (NULL != sock_ev_write_struct)
	{
//      event_del(sock_ev_write_struct->write_ev);//因为写事件没用EV_PERSIST，故不用event_del
        if(NULL != sock_ev_write_struct->write_ev)
            event_free(sock_ev_write_struct->write_ev);

        if(NULL != sock_ev_write_struct->buffer)
            delete sock_ev_write_struct->buffer;

        delete sock_ev_write_struct;
    }
}

void destroy_sock_ev_read(struct sock_ev_read* sock_ev_read_struct)
{
	if(NULL != sock_ev_read_struct)
	{
		event_del(sock_ev_read_struct->read_ev);
		if(NULL != sock_ev_read_struct->read_ev)
			event_free(sock_ev_read_struct->read_ev);

		event_base_free(sock_ev_read_struct->base);
		delete sock_ev_read_struct;
	}
}

void ServerThread::SetAddressPort(unsigned short nPort)
{
	m_nPort = nPort;
	m_nMaxPort = m_nPort + 50;
}

void ServerThread::EnableFloatPort(bool bEnable)
{
	m_bFloatPort = bEnable;
}

bool ServerThread::Initialize(long lInitThreadNum/* = 4*/, long lMaxThreadNum/* = 2*4*/, long lAvailLowNum/* = 0*/, long lAvailHighNum/* = 4*/)
{
	WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2) , &wsaData) != 0) 
    {
        return false;
    }
	//new event
    m_base = event_base_new();

	//create thread
	m_hThread = _beginthreadex(NULL, 0, ServerThread::ThreadFunction, reinterpret_cast<void*>(this), 0, &m_uThreadId);
	if (!m_hThread) 
	{
		return false;
	}

	ThreadPoolSingleton::GetInstance()->Initialize(lInitThreadNum, lMaxThreadNum, lAvailLowNum, lAvailHighNum);
	
	return true;
}

void ServerThread::Exit(void)
{
	if (!m_hThread) return;

	//exit event
	if (m_base)
	{
		event_base_loopexit(m_base, NULL);
		event_base_free(m_base);
		m_base = NULL;
	}
	
	if (WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hThread, 500))
		TerminateThread((HANDLE)m_hThread, 1);

	if (m_evlisten)
	{
		event_free(m_evlisten);
		m_evlisten = NULL;
	}
	closesocket(m_listener);

	ThreadPoolSingleton::GetInstance()->Exit();
}

unsigned int WINAPI ServerThread::ThreadFunction(void* pContext)
{
	ServerThread *pThread = static_cast<ServerThread*>(pContext);
	
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

unsigned int ServerThread::Execute()
{
    m_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//IPPROTO_TCP
    int yes = 1;
    setsockopt(m_listener, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(int));
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;//inet_addr(m_szIp);
	while (1)
	{
		server.sin_port = htons(m_nPort);
		LOG_TRACE("try to bind port[%d]...", m_nPort);
		if (bind(m_listener, (struct sockaddr*)&server, sizeof(struct sockaddr)) != 0)
		{
			LOG_TRACE("bind port[%d] failed, error[%d]:%s", m_nPort, WSAGetLastError(), GetLastErrorMessage(WSAGetLastError()).c_str());
			if (!m_bFloatPort) return -1;

			while (PortUsedTCP(++m_nPort));
			if (m_nPort > m_nMaxPort)
			{
				LOG_TRACE("bind port error");
				return -1;
			}
			continue;
		}
		LOG_TRACE("bind port[%d] success", m_nPort);
		break;
	}
    if (listen(m_listener, BACKLOG) != 0)
	{
		LOG_TRACE("listen failed, error[%d]:%s", WSAGetLastError(), GetLastErrorMessage(WSAGetLastError()).c_str());
		return -1;
	}

	m_evlisten = event_new(m_base, m_listener, EV_READ | EV_PERSIST, ServerThread::on_accept, this);
    event_add(m_evlisten, NULL);

	LOG_TRACE("server start OK!");
    event_base_dispatch(m_base);
	LOG_TRACE("server exit OK!");

	return 0;
}

unsigned int ServerThread::ThreadAcceptFunction(void* pContext)
{
	struct accept_param *param = (struct accept_param *)pContext;
	if (!param)
	{
		LOG_TRACE("accept_param is null");
		return 1;
	}
	int sockfd = param->sockfd;
	if (sockfd < 0)
	{
		LOG_TRACE("socket error, sockfd=%d", sockfd);
		return 0;
	}
	//初始化base,写事件和读事件
	struct event_base* base = event_base_new();

	//仅仅是为了动态创建一个event结构体
	struct event *read_ev = event_new(NULL, -1, 0, NULL, NULL);
	
	struct sock_ev_read* event_read = new sock_ev_read;
	event_read->base = base;
	event_read->read_ev = read_ev;
	event_read->pServer = param->pServer;

	//将动态创建的结构体作为event的回调参数
	event_assign(read_ev, base, sockfd, EV_READ | EV_PERSIST, ServerThread::on_read, (void*)event_read);
	event_add(read_ev, NULL);

	//开始libevent的loop循环
	event_base_dispatch(base);

	LOG_TRACE("event_base_dispatch() stopped for sock[%d]", sockfd);

	destroy_sock_ev_read(event_read);
	closesocket(sockfd);
	delete pContext;
	return 0;
}

void ServerThread::on_accept(int sock, short event, void* arg)
{
	int sockfd;
	struct sockaddr_in client;
	int len = sizeof(client);
	sockfd = ::accept(sock, (struct sockaddr*)&client, &len);
	LOG_TRACE("accept a client %d", sockfd);

	struct accept_param *param = new accept_param;
	param->pServer = (ServerThread *)arg;
	param->sockfd = sockfd;
	ThreadPoolSingleton::GetInstance()->SubmitTask(ServerThread::ThreadAcceptFunction, param);
}

void ServerThread::on_read(int sock, short event, void* arg)
{
	LOG_TRACE("called, sock=%d", sock);
	if (!arg)
	{
		LOG_TRACE("error, arg is null");
		return;
	}
	struct sock_ev_read* event_read = (struct sock_ev_read*)arg;
	long lPackageSize = 0;

	long lRecvSize = recv(sock, (char *)&lPackageSize, sizeof(lPackageSize), 0);

	if(lRecvSize <= 0) //socket关闭 或 出错
	{
		if (lRecvSize == 0)
			LOG_TRACE("socket[%d] connect is closed", sock);
		else
			LOG_TRACE("socket[%d] receive failed, error[%d]:%s", sock, WSAGetLastError(), GetLastErrorMessage(WSAGetLastError()).c_str());
		event_base_loopexit(event_read->base, NULL); //停止loop循环
		return;
	}
	Buffer buffer;
	char *pBuf = (char *)buffer.GetBufferSetSize(lPackageSize);
	lRecvSize = 0;
	while (lRecvSize < lPackageSize)
	{
		long lRecv = recv(sock, pBuf+lRecvSize, lPackageSize-lRecvSize, 0);
		if(lRecv <= 0) //socket关闭 或 出错
		{
			if (lRecvSize == 0)
				LOG_TRACE("socket[%d] connect is closed", sock);
			else
				LOG_TRACE("socket[%d] receive failed, error[%d]:%s", sock, WSAGetLastError(), GetLastErrorMessage(WSAGetLastError()).c_str());
			event_base_loopexit(event_read->base, NULL); //停止loop循环
			return;
		}
		lRecvSize += lRecv;
	}

	LOG_TRACE("socket[%d] received buffer of %d size", sock, buffer.GetSize());
	struct socket_param param = { sock, event_read->base };
	if (!event_read->pServer->IsTestClient(&param, buffer))
		event_read->pServer->OnReadEvent(&param, buffer);
	
	LOG_TRACE("finished, sock=%d", sock);
}

void ServerThread::on_write(int sock, short event, void* arg)
{
	LOG_TRACE("called, sock=%d", sock);
	if (!arg)
	{
		LOG_TRACE("error, arg is null");
		return;
	}
	struct sock_ev_write* event_write = (struct sock_ev_write*)arg;

	long lSize = event_write->buffer->GetSize();

	//先发送长度
	send(sock, (const char *)&lSize, sizeof(lSize), 0);

	send(sock, (const char *)event_write->buffer->GetBuffer(), lSize, 0);
	LOG_TRACE("socket[%d] send buffer of %d size", sock, lSize);

	destroy_sock_ev_write(event_write);
	LOG_TRACE("finished, sock=%d", sock);
}

void ServerThread::OnReadEvent(void *socket, const Buffer &bufRecv)
{
}

void ServerThread::Send(void *socket, const Buffer &bufSend)
{
	if (!socket) return;
	struct sock_ev_write *event_write = new sock_ev_write;
	event_write->buffer = new Buffer(bufSend);
	struct event *write_ev = event_new(NULL, -1, 0, NULL, NULL);
	event_write->write_ev = write_ev;
	struct socket_param *param = (struct socket_param *)socket;
	event_assign(write_ev, param->base, param->socket, EV_WRITE, ServerThread::on_write, event_write);
	event_add(write_ev, NULL);
}

void ServerThread::Disconnect(void *socket)
{
	if (!socket) return;
	struct socket_param *param = (struct socket_param *)socket;
	event_base_loopexit(param->base, NULL); //停止loop循环
}

bool ServerThread::IsTestClient(void *socket, const Buffer &bufRecv)
{
	if (bufRecv.GetSize() == 8)
	{
		string strData((char *)bufRecv.GetBuffer(), bufRecv.GetSize());
		//暗号 月薪上万 工资日结
		if (strData == "月薪上万")
		{
			Buffer buf;
			buf.AppendStringA("工资日结");
			Send(socket, buf);
			return true;
		}
	}
	return false;
}

}}
