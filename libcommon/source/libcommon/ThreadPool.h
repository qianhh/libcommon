#pragma once
#include "Singleton.h"

/**
 ** �̳߳�
 ** designed by eric,2016/08/20
 **/
namespace eric { namespace common {
#ifndef STRICT
typedef void *HANDLE;
#endif
class ThreadPool
{
public:
	ThreadPool(void);
	~ThreadPool(void);

	/**
	 ** ��ʼ���̳߳� ע�⣺Ĭ��ֵ���ᶯ̬�����̳߳���Ŀ
	 ** @param[in] lInitThreadNum		��ʼ�������̸߳���
	 ** @param[in] lMaxThreadNum		��������̸߳���
	 ** @param[in] lAvailLowNum			��Ϳ����̸߳���
	 ** @param[in] lAvailHighNum		��߿����̸߳���
	 **/
	void Initialize(long lInitThreadNum = 4, long lMaxThreadNum = 2*4, long lAvailLowNum = 0, long lAvailHighNum = 4);
	/**
	 ** �ر��̳߳�
	 **/
	void Exit(void);
	/**
	 ** �����̳߳ص��߳���
	 ** @param[in] lNum		�������߳�����lNum>0 ���ӣ�lNum<0 ����
	 **/
	void AdjustSize(long lNum);
	/**
	 ** �ύ����
	 ** @param[in] pFunc		������ָ��
	 ** @param[in] pParam		����������
	 **/
	void SubmitTask(unsigned int (*pFunc)(void *), void *pParam);

private:
	unsigned int Execute(void *pParam); //ִ�к���
	static unsigned int __stdcall ThreadFunction(void* pContext); //�̺߳���

	void AutoAdjustPoolSize(void); //����Ӧ�̳߳ش�С

private:
	class TaskMgr;
	TaskMgr *m_pTaskMgr;	//�������
	class ThreadMgr;
	ThreadMgr *m_pThreadMgr;//�̹߳���

	HANDLE m_SemaphoreTask;	//�����ź�
	HANDLE m_SemaphoreDel;	//ɾ���߳��ź�
	HANDLE m_EventExit;		//�ر��̳߳��¼�
};


/**
 ** �̳߳أ�����ģʽ
 ** designed by eric,2016/08/20
 **/
class ThreadPoolSingleton : public CSingleton<ThreadPool>
{
	friend class CSingleton<ThreadPool>;
private:
	ThreadPoolSingleton(void);
	~ThreadPoolSingleton(void);
};

}}
