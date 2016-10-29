#pragma once

/**
 ** �ͻ��������ܵ�
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
	 ** ��ʼ��
	 **/
	bool Initialize(wchar_t *pszPipeName = L"\\\\.\\pipe\\AppService");
	/**
	 ** �ȴ������ܵ���һ��ʵ����Ч
	 ** @param[in] nTimeOut		��ʱʱ��
	 ** @retval					true:����ڳ�ʱʱ��ǰ�ܵ���һ��ʵ����Ч�����ط�0
	 ** @retval					false:�����ʱʱ����û��һ����Ч��ʵ��������0
	 **/
	bool WaitPipe(unsigned int nTimeOut = NMPWAIT_NOWAIT);
	/**
	 ** �������ܵ�
	 ** @retval					true:�ɹ�
	 ** @retval					false:ʧ��
	 **/
	bool OpenPipe(void);
	/**
	 ** �رչܵ�
	 **/
	void ClosePipe(void);
	/**
	 ** ���������̷������ݣ���������
	 ** @param[in] bufSend		���͵�����
	 ** @retval					true:�ɹ�
	 ** @retval					false:ʧ��
	 **/
	bool Send(const Buffer &bufSend);
	/**
	 ** �������¼������̵߳���
	 ** @param[in] bufRecv		���յ�������
	 **/
	virtual void OnReadEvent(const Buffer &bufRecv);

private:
	unsigned int Execute(void); //ִ�к���
	static unsigned int WINAPI ThreadFunction(void* pContext);

private:
	uintptr_t	m_hPipe;			//�����ܵ�
	uintptr_t	m_hThread;
	wchar_t		m_szPipeName[64];
};

}}
