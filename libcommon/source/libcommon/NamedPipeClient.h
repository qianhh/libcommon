#pragma once

/**
 ** 客户端命名管道
 ** designed by eric,2016/10/24
 **/
namespace eric { namespace common {
class Buffer;
class NamedPipeClient
{
public:
	NamedPipeClient(void);
	virtual ~NamedPipeClient(void);

	/**
	 ** 初始化
	 **/
	bool Initialize(wchar_t *pszPipeName = L"\\\\.\\pipe\\AppService");
	/**
	 ** 等待命名管道的一个实例有效
	 ** @param[in] nTimeOut		超时时间
	 ** @retval					true:如果在超时时间前管道的一个实例有效，返回非0
	 ** @retval					false:如果超时时间内没有一个有效的实例，返回0
	 **/
	bool WaitPipe(unsigned int nTimeOut = NMPWAIT_NOWAIT);
	/**
	 ** 打开命名管道
	 ** @retval					true:成功
	 ** @retval					false:失败
	 **/
	bool OpenPipe(void);
	/**
	 ** 关闭管道
	 **/
	void ClosePipe(void);
	/**
	 ** 向其它进程发送数据，阻塞函数
	 ** @param[in] bufSend		发送的数据
	 ** @retval					true:成功
	 ** @retval					false:失败
	 **/
	bool Send(const Buffer &bufSend);
	/**
	 ** 读数据事件处理，线程调用
	 ** @param[in] bufRecv		接收到的数据
	 **/
	virtual void OnReadEvent(const Buffer &bufRecv);

private:
	unsigned int Execute(void); //执行函数
	static unsigned int WINAPI ThreadFunction(void* pContext);

private:
	uintptr_t	m_hPipe;			//命名管道
	uintptr_t	m_hThread;
	wchar_t		m_szPipeName[64];
};

}}
