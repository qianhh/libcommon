#pragma once

/**
 ** 服务端命名管道线程
 ** designed by eric,2016/10/24
 **/
namespace eric { namespace common {
class Buffer;
class NamedPipeServer
{
public:
	NamedPipeServer(void);
	virtual ~NamedPipeServer(void);

	/**
	 ** 初始化线程
	 ** @param[in] pszPipeName			命名管道名字
	 **/
	bool Initialize(wchar_t *pszPipeName = L"\\\\.\\pipe\\AppService");
	/**
	 ** 退出服务
	 **/
	void Exit(void);
	/**
	 ** 读数据事件处理，线程调用
	 ** @param[in] socket				pipe
	 ** @param[in] bufRecv				接收到的数据
	 **/
	virtual void OnReadEvent(void *pipe, const Buffer &bufRecv);
	/**
	 ** 发送数据
	 ** @param[in] pipe					pipe
	 ** @param[in] bufSend				待发送的数据
	 **/
	static bool Send(void *pipe, const Buffer &bufSend);

public:
	//内部使用
	void GetAnswerToRequest(void *pipe);
	BOOL CreateAndConnectInstance(void *lpoOverlap); 

private:
	unsigned int Execute(void); //执行函数
	static unsigned int WINAPI ThreadFunction(void* pContext);

private:
	uintptr_t	m_hThread;
	bool		m_bExitEvent;
	uintptr_t	m_hPipe;
	wchar_t		m_szPipeName[64];
};

}}
