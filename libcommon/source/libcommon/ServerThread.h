#pragma once

/**
 ** socket�����߳�, ʹ�����̳߳ش�������
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
	 ** ��ʼ������ ע�⣺Ĭ��ֵ���ᶯ̬�����̳߳���Ŀ
	 ** @param[in] lInitThreadNum		��ʼ�������̸߳���
	 ** @param[in] lMaxThreadNum		��������̸߳���
	 ** @param[in] lAvailLowNum			��Ϳ����̸߳���
	 ** @param[in] lAvailHighNum		��߿����̸߳���
	 **/
	bool Initialize(long lInitThreadNum = 4, long lMaxThreadNum = 2*4, long lAvailLowNum = 0, long lAvailHighNum = 4);
	/**
	 ** �˳�����
	 **/
	void Exit(void);
	/**
	 ** ���ö˿�
	 ** @param[in] nPort				�˿�
	 **/
	void SetAddressPort(unsigned short nPort);
	/**
	 ** ���ø����˿� ��SetAddressPort���õĶ˿ڿ�ʼ����bind����port��һ����Χ��ֱ��bind�ɹ�Ϊֹ��Ĭ�ϲ�����
	 ** @param[in] bEnable				�Ƿ�����
	 **/
	void EnableFloatPort(bool bEnable);
	/**
	 ** �������¼��������̵߳���
	 ** @param[in] socket				socket
	 ** @param[in] bufRecv				���յ�������
	 **/
	virtual void OnReadEvent(void *socket, const Buffer &bufRecv);
	/**
	 ** ��������
	 ** @param[in] socket				socket
	 ** @param[in] bufSend				�����͵�����
	 **/
	static void Send(void *socket, const Buffer &bufSend);
	/**
	 ** �ر�����
	 ** @param[in] socket				socket
	 **/
	static void Disconnect(void *socket);

private:
	unsigned int Execute(void); //ִ�к���
	static unsigned int WINAPI ThreadFunction(void* pContext);
	static unsigned int ThreadAcceptFunction(void* pContext);
	static void on_accept(int sock, short event, void* arg);
	static void on_read(int sock, short event, void* arg);
	static void on_write(int sock, short event, void* arg);

	bool IsTestClient(void *socket, const Buffer &bufRecv); //���Կͻ�������
	
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
