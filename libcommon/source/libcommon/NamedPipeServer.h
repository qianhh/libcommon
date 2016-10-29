#pragma once

/**
 ** ����������ܵ��߳�
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
	 ** ��ʼ���߳�
	 ** @param[in] pszPipeName			�����ܵ�����
	 **/
	bool Initialize(wchar_t *pszPipeName = L"\\\\.\\pipe\\AppService");
	/**
	 ** �˳�����
	 **/
	void Exit(void);
	/**
	 ** �������¼������̵߳���
	 ** @param[in] socket				pipe
	 ** @param[in] bufRecv				���յ�������
	 **/
	virtual void OnReadEvent(void *pipe, const Buffer &bufRecv);
	/**
	 ** ��������
	 ** @param[in] pipe					pipe
	 ** @param[in] bufSend				�����͵�����
	 **/
	static bool Send(void *pipe, const Buffer &bufSend);

public:
	//�ڲ�ʹ��
	void GetAnswerToRequest(void *pipe);
	BOOL CreateAndConnectInstance(void *lpoOverlap); 

private:
	unsigned int Execute(void); //ִ�к���
	static unsigned int WINAPI ThreadFunction(void* pContext);

private:
	uintptr_t	m_hThread;
	bool		m_bExitEvent;
	uintptr_t	m_hPipe;
	wchar_t		m_szPipeName[64];
};

}}
