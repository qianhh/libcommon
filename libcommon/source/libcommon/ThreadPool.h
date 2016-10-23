#pragma once
#include "Singleton.h"

/**
 ** 线程池
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
	 ** 初始化线程池 注意：默认值不会动态调整线程池数目
	 ** @param[in] lInitThreadNum		初始开启的线程个数
	 ** @param[in] lMaxThreadNum		最大开启的线程个数
	 ** @param[in] lAvailLowNum			最低可用线程个数
	 ** @param[in] lAvailHighNum		最高可用线程个数
	 **/
	void Initialize(long lInitThreadNum = 4, long lMaxThreadNum = 2*4, long lAvailLowNum = 0, long lAvailHighNum = 4);
	/**
	 ** 关闭线程池
	 **/
	void Exit(void);
	/**
	 ** 调整线程池的线程数
	 ** @param[in] lNum		调整的线程数，lNum>0 增加，lNum<0 减少
	 **/
	void AdjustSize(long lNum);
	/**
	 ** 提交任务
	 ** @param[in] pFunc		任务函数指针
	 ** @param[in] pParam		任务函数参数
	 **/
	void SubmitTask(unsigned int (*pFunc)(void *), void *pParam);

private:
	unsigned int Execute(void *pParam); //执行函数
	static unsigned int __stdcall ThreadFunction(void* pContext); //线程函数

	void AutoAdjustPoolSize(void); //自适应线程池大小

private:
	class TaskMgr;
	TaskMgr *m_pTaskMgr;	//任务管理
	class ThreadMgr;
	ThreadMgr *m_pThreadMgr;//线程管理

	HANDLE m_SemaphoreTask;	//任务信号
	HANDLE m_SemaphoreDel;	//删除线程信号
	HANDLE m_EventExit;		//关闭线程池事件
};


/**
 ** 线程池，单例模式
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
