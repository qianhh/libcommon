#pragma once

/**
 ** socket�ͻ��߳�
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
	 ** ��ʼ���߳�
	 **/
	bool Initialize(void);
	/**
	 ** �˳��߳�
	 **/
	void Exit(void);
	/**
	 ** ����ip
	 ** @param[in] pszIp				ip
	 **/
	void SetAddressIp(const char *pszIp);
	/**
	 ** ���ö˿�
	 ** @param[in] nPort				�˿�
	 **/
	void SetAddressPort(unsigned short nPort);
	/**
	 ** ����ip�˿�
	 ** @param[in] pszIp				ip
	 ** @param[in] nPort				�˿�
	 **/
	void SetAddress(const char *pszIp, unsigned short nPort);
	/**
	 ** ���ø����˿� �����õĶ˿ڿ�ʼ����connect����port��һ����Χ��ֱ��connect�ɹ�Ϊֹ��Ĭ�ϲ�����
	 ** @param[in] bEnable				�Ƿ�����
	 **/
	void EnableFloatPort(bool bEnable);
	/**
	 ** ���ӷ����
	 **/
	bool Connect(void);
	/**
	 ** �ر�����
	 **/
	void Disconnect(void);
	/**
	 ** �������¼����������߳��е���
	 ** @param[in] bufRecv				���յ�������
	 **/
	virtual void OnReadEvent(const Buffer &bufRecv);
	/**
	 ** ��������
	 ** @param[in] bufSend				�����͵�����
	 **/
	bool Send(const Buffer &bufSend);

private:
	unsigned int Execute(void); //ִ�к���
	static unsigned int __stdcall ThreadFunction(void* pContext);
	bool TestConnect(void); //��������
	
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
