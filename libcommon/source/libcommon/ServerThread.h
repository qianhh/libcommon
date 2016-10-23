#pragma once

/**
 ** socket服务线程, 使用了线程池处理连接
 ** designed by eric,2016/08/17
 ** modified by eric,2016/10/21
 **/
struct event_base;
struct event;
namespace eric { namespace common {
class Buffer;
class ServerThread
{
public:
	ServerThread(void);
	virtual ~ServerThread(void);

	/**
	 ** 初始化服务 注意：默认值不会动态调整线程池数目
	 ** @param[in] lInitThreadNum		初始开启的线程个数
	 ** @param[in] lMaxThreadNum		最大开启的线程个数
	 ** @param[in] lAvailLowNum			最低可用线程个数
	 ** @param[in] lAvailHighNum		最高可用线程个数
	 **/
	bool Initialize(long lInitThreadNum = 4, long lMaxThreadNum = 2*4, long lAvailLowNum = 0, long lAvailHighNum = 4);
	/**
	 ** 退出服务
	 **/
	void Exit(void);
	/**
	 ** 设置端口
	 ** @param[in] nPort				端口
	 **/
	void SetAddressPort(unsigned short nPort);
	/**
	 ** 启用浮动端口 从SetAddressPort设置的端口开始尝试bind，在port的一定范围内直到bind成功为止，默认不启用
	 ** @param[in] bEnable				是否启用
	 **/
	void EnableFloatPort(bool bEnable);
	/**
	 ** 读数据事件处理，多线程调用
	 ** @param[in] socket				socket
	 ** @param[in] bufRecv				接收到的数据
	 **/
	virtual void OnReadEvent(void *socket, const Buffer &bufRecv);
	/**
	 ** 发送数据
	 ** @param[in] socket				socket
	 ** @param[in] bufSend				待发送的数据
	 **/
	static void Send(void *socket, const Buffer &bufSend);
	/**
	 ** 关闭连接
	 ** @param[in] socket				socket
	 **/
	static void Disconnect(void *socket);

private:
	unsigned int Execute(void); //执行函数
	static unsigned int WINAPI ThreadFunction(void* pContext);
	static unsigned int ThreadAcceptFunction(void* pContext);
	static void on_accept(int sock, short event, void* arg);
	static void on_read(int sock, short event, void* arg);
	static void on_write(int sock, short event, void* arg);

	bool IsTestClient(void *socket, const Buffer &bufRecv); //测试客户端连接
	
private:
	uintptr_t			m_hThread;		//thread handle
	unsigned int		m_uThreadId;	//thread id
	struct event_base	*m_base;		//event_base pointer
	unsigned short		m_nPort;		//port begain
	unsigned short		m_nMaxPort;		//max port 
	bool				m_bFloatPort;	//enable float port
	struct event		*m_evlisten;	//event listen pointer
	int					m_listener;		//listen socket

};

}}
