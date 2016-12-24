#pragma once

/**
 ** socket客户线程
 ** designed by eric,2016/08/22
 ** modified by eric,2016/10/21
 **/
namespace eric { namespace common {
class Buffer;
class Event;
class ClientThread
{
public:
	ClientThread(void);
	virtual ~ClientThread(void);

	/**
	 ** 初始化线程
	 **/
	bool Initialize(void);
	/**
	 ** 退出线程
	 **/
	void Exit(void);
	/**
	 ** 设置ip
	 ** @param[in] pszIp				ip
	 **/
	void SetAddressIp(const char *pszIp);
	/**
	 ** 设置端口
	 ** @param[in] nPort				端口
	 **/
	void SetAddressPort(unsigned short nPort);
	/**
	 ** 设置ip端口
	 ** @param[in] pszIp				ip
	 ** @param[in] nPort				端口
	 **/
	void SetAddress(const char *pszIp, unsigned short nPort);
	/**
	 ** 启用浮动端口 从设置的端口开始尝试connect，在port的一定范围内直到connect成功为止，默认不启用
	 ** @param[in] bEnable				是否启用
	 **/
	void EnableFloatPort(bool bEnable);
	/**
	 ** 连接服务端
	 **/
	bool Connect(void);
	/**
	 ** 关闭连接
	 **/
	void Disconnect(void);
	/**
	 ** 读数据事件处理，接收线程中调用
	 ** @param[in] bufRecv				接收到的数据
	 **/
	virtual void OnReadEvent(const Buffer &bufRecv);
	/**
	 ** 发送数据
	 ** @param[in] bufSend				待发送的数据
	 **/
	bool Send(const Buffer &bufSend);

private:
	unsigned int Execute(void); //执行函数
	static unsigned int __stdcall ThreadFunction(void* pContext);
	bool TestConnect(void); //测试连接
	
private:
	uintptr_t			m_hThread;			//thread handle
	unsigned int		m_uThreadId;		//thread id
	char				m_szIp[32];			//ip
	unsigned short		m_nPort;			//port begain
	unsigned short		m_nMaxPort;			//max port 
	bool				m_bFloatPort;		//enable float port
	uintptr_t			m_sockClient;		//client socket
	bool				m_bExit;			//thread exit flag
	Event				*m_pEventConnect;	//connect event
};

}}
