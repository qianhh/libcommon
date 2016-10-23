#include "StdAfx.h"
#include "ThreadPool.h"
#include "CriticalSectionLock.h"
#include <process.h>
#include <queue>
#include <map>
#include <assert.h>
#include <windows.h>
using namespace std;

#define DELETE_HANDLE(h) \
	if (h) { CloseHandle(h); h = NULL; }

typedef struct tagTaskItem
{
	unsigned int (*_pFunc)(void *);	//函数指针
	void *_pParam;					//函数参数
}TASK_ITEM, *PTASK_ITEM;

typedef struct tagThreadItem
{
	HANDLE		_hThread;
	UINT		_uThreadId;
	ThreadPool *_pThreadPool;
}THREAD_ITEM, *PTHREAD_ITEM;

class ThreadPool::TaskMgr
{
public:
	typedef queue<PTASK_ITEM> TASK_TYPE;

	TaskMgr(void)
	{
	}

	~TaskMgr(void)
	{
		ReleaseAll();
	}

	void AddTask(unsigned int (*pFunc)(void *), void *pParam)
	{
		CriticalSectionAutoLock lock(m_cs);
		PTASK_ITEM pItem = NULL;
		if (!m_IdleQueue.empty())
		{
			pItem = m_IdleQueue.front();
			m_IdleQueue.pop();
		}
		else
		{
			pItem = new TASK_ITEM;
		}
		if (pItem)
		{
			pItem->_pFunc = pFunc;
			pItem->_pParam = pParam;
			m_BusyQueue.push(pItem);
		}
	}

	PTASK_ITEM PopFront(void)
	{
		CriticalSectionAutoLock lock(m_cs);
		if (!m_BusyQueue.empty())
		{
			PTASK_ITEM pItem = m_BusyQueue.front();
			m_BusyQueue.pop();
			return pItem;
		}
		assert(0);
		return NULL;
	}

	void PushToIdle(PTASK_ITEM pItem)
	{
		CriticalSectionAutoLock lock(m_cs);
		m_IdleQueue.push(pItem);
	}

	void ReleaseAll(void)
	{
		CriticalSectionAutoLock lock(m_cs);
		PTASK_ITEM pItem = NULL;
		while (!m_BusyQueue.empty())
		{
			pItem = m_BusyQueue.front();
			DELETE_PTR(pItem);
			m_BusyQueue.pop();
		}
		while (!m_IdleQueue.empty())
		{
			pItem = m_IdleQueue.front();
			DELETE_PTR(pItem);
			m_IdleQueue.pop();
		}
	}

private:
	TASK_TYPE m_BusyQueue;
	TASK_TYPE m_IdleQueue;
	CriticalSectionLock m_cs;	//临界区
};

class ThreadPool::ThreadMgr
{
public:
	typedef map<UINT, PTHREAD_ITEM> THREAD_TYPE;
	ThreadMgr(void) : m_lInitNum(0), m_lMaxNum(0), m_lAvailLowNum(0), m_lAvailHighNum(0), m_lAvailNum(0)
	{
	}

	~ThreadMgr(void) 
	{
		CloseAllThread();
	}

	long CreateNewThread(long lNum, ThreadPool *pThreadPool)
	{
		CriticalSectionAutoLock lock(m_cs);
		PTHREAD_ITEM pThread = NULL;
		long lSuccessNum = 0;
		for (long i = 0; i < lNum; ++i)
		{
			pThread = new THREAD_ITEM;
			if (!pThread) continue;
			memset(pThread, 0, sizeof(THREAD_ITEM));
			pThread->_pThreadPool = pThreadPool;
			pThread->_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadPool::ThreadFunction, reinterpret_cast<void*>(pThread), 0, &pThread->_uThreadId);
			if (!pThread->_hThread)
			{
				DELETE_PTR(pThread);
			}
			else
			{
				m_ThreadMap[pThread->_uThreadId] = pThread;
				++lSuccessNum;
				IncreaseAvailNumber();
			}
		}
		return lSuccessNum;
	}

	void CloseAllThread(void)
	{
		CriticalSectionAutoLock lock(m_cs);
		long lCount = m_ThreadMap.size();
		if (lCount <= 0) return;
		long i = 0;
		//关闭线程
		if (HANDLE *hThreads = new HANDLE[lCount])
		{
			for (THREAD_TYPE::const_iterator it = m_ThreadMap.begin(); it != m_ThreadMap.end(); ++it)
				hThreads[i] = it->second->_hThread;
			if (WAIT_TIMEOUT == ::WaitForMultipleObjects(lCount, hThreads, TRUE, 500))
			{
				for (i = 0; i < lCount; ++i)
					TerminateProcess(hThreads[i], 1);
			}
			DELETE_PTRS(hThreads);
		}
		else
		{
			for (THREAD_TYPE::const_iterator it = m_ThreadMap.begin(); it != m_ThreadMap.end(); ++it)
				TerminateProcess(it->second->_hThread, 1);
		}
		//释放线程数据
		PTHREAD_ITEM pThread = NULL;
		for (THREAD_TYPE::const_iterator it = m_ThreadMap.begin(); it != m_ThreadMap.end(); ++it)
		{
			pThread = it->second;
			DELETE_HANDLE(pThread->_hThread);
			DELETE_PTR(pThread);
			DecreaseAvailNumber();
		}
		m_ThreadMap.clear();
	}

	void DestroyThread(UINT uThreadId)
	{
		CriticalSectionAutoLock lock(m_cs);
		THREAD_TYPE::iterator it = m_ThreadMap.find(uThreadId);
		if (it != m_ThreadMap.end())
		{
			PTHREAD_ITEM pThread = it->second;
			DELETE_HANDLE(pThread->_hThread);
			DELETE_PTR(pThread);
			m_ThreadMap.erase(it);
			DecreaseAvailNumber();
		}
		else
		{
			assert(0);
		}
	}

	long GetThreadSize(void)
	{
		CriticalSectionAutoLock lock(m_cs);
		return m_ThreadMap.size();
	}

	inline void SetInitNumber(long lNum)
	{
		m_lInitNum = lNum;
	}

	inline long GetInitNumber(void) const
	{
		return m_lInitNum;
	}

	inline void SetMaxNumber(long lNum)
	{
		m_lMaxNum = lNum;
	}

	inline long GetMaxNumber(void) const
	{
		return m_lMaxNum;
	}

	inline void SetAvailLowNumber(long lNum)
	{
		m_lAvailLowNum = lNum;
	}

	inline long GetAvailLowNumber(void) const
	{
		return m_lAvailLowNum;
	}

	inline void SetAvailHighNumber(long lNum)
	{
		m_lAvailHighNum = lNum;
	}

	inline long GetAvailHighNumber(void) const
	{
		return m_lAvailHighNum;
	}

	inline void IncreaseAvailNumber(void)
	{
		::InterlockedIncrement(&m_lAvailNum);
	}

	inline void DecreaseAvailNumber(void)
	{
		::InterlockedDecrement(&m_lAvailNum);
	}

	inline long GetAvailNumber(void) const
	{
		return m_lAvailNum;
	}

private:
	THREAD_TYPE m_ThreadMap;
	CriticalSectionLock m_cs;	//临界区
	long m_lInitNum;			//初始线程数
	long m_lMaxNum;				//最大线程数
	long m_lAvailLowNum;		//最低可用线程数 m_lAvailNum<m_lAvailLowNum 需要创建线程
	long m_lAvailHighNum;		//最高可用线程数 m_lAvailNum>m_lAvailHighNum 需要销毁线程
	long m_lAvailNum;			//当前可用线程数 m_lAvailLowNum<=m_lAvailNum<=m_lAvailHighNum 不需要创建销毁线程
};

ThreadPool::ThreadPool(void) : m_pTaskMgr(new TaskMgr), m_pThreadMgr(new ThreadMgr)
	, m_SemaphoreTask(NULL), m_SemaphoreDel(NULL), m_EventExit(NULL)
{
}

ThreadPool::~ThreadPool(void)
{
	DELETE_PTR(m_pTaskMgr);
	DELETE_PTR(m_pThreadMgr);

	DELETE_HANDLE(m_SemaphoreTask);
	DELETE_HANDLE(m_SemaphoreDel);
	DELETE_HANDLE(m_EventExit);
}

void ThreadPool::Initialize(long lInitThreadNum/* = 4*/, long lMaxThreadNum/* = 2*4*/, long lAvailLowNum/* = 0*/, long lAvailHighNum/* = 4*/)
{
	m_SemaphoreTask = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
	m_SemaphoreDel = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
	m_EventExit = CreateEvent(NULL, TRUE, FALSE, NULL);

	assert(lMaxThreadNum >= lInitThreadNum);
	assert(lAvailHighNum >= lInitThreadNum);
	assert(lAvailLowNum <= lInitThreadNum);
	m_pThreadMgr->SetInitNumber(lInitThreadNum);
	m_pThreadMgr->SetMaxNumber(lMaxThreadNum);
	m_pThreadMgr->SetAvailLowNumber(lAvailLowNum);
	m_pThreadMgr->SetAvailHighNumber(lAvailHighNum);
	AdjustSize(lInitThreadNum);
}

void ThreadPool::Exit(void)
{
	SetEvent(m_EventExit);
	m_pThreadMgr->CloseAllThread();
}

void ThreadPool::AdjustSize(long lNum)
{
	if (lNum > 0)
	{
		m_pThreadMgr->CreateNewThread(lNum, this);
	}
	else
	{
		lNum = -lNum;
		long lThreadNum = m_pThreadMgr->GetThreadSize();
		ReleaseSemaphore(m_SemaphoreDel, lNum>lThreadNum?lThreadNum:lNum, NULL);
	}
}

void ThreadPool::SubmitTask(unsigned int (*pFunc)(void *), void *pParam)
{
	m_pTaskMgr->AddTask(pFunc, pParam);
	AutoAdjustPoolSize();
	ReleaseSemaphore(m_SemaphoreTask, 1, NULL);
}

void ThreadPool::AutoAdjustPoolSize(void)
{
	long lInitNum = m_pThreadMgr->GetInitNumber();
	long lAvailNum = m_pThreadMgr->GetAvailNumber();
	long lIncreaseNum = m_pThreadMgr->GetAvailLowNumber() - lAvailNum;
	if (lIncreaseNum > 0)
	{
		long lThreadSize = m_pThreadMgr->GetThreadSize();
		if (lThreadSize + lIncreaseNum > m_pThreadMgr->GetMaxNumber())
			AdjustSize(m_pThreadMgr->GetMaxNumber() - lThreadSize);
		else
			AdjustSize(lIncreaseNum);
	}
	else if (lAvailNum > m_pThreadMgr->GetAvailHighNumber())
	{
		AdjustSize(-(lAvailNum - lInitNum)); //删除(lAvailNum - lInitNum)个线程
	}
}

unsigned int ThreadPool::Execute(void *pParam)
{
	PTHREAD_ITEM pThreadData = static_cast<PTHREAD_ITEM>(pParam);
	ThreadPool *pThreadPool = pThreadData->_pThreadPool;
	TaskMgr *pTaskMgr = pThreadPool->m_pTaskMgr;
	ThreadMgr *pThreadMgr = pThreadPool->m_pThreadMgr;
	DWORD dwResult = 0;
	PTASK_ITEM pTaskItem = NULL;
	HANDLE hObjects[3] = 
	{
		pThreadPool->m_EventExit,
		pThreadPool->m_SemaphoreTask,
		pThreadPool->m_SemaphoreDel
	};
	bool bRun = true;
	while (bRun)
	{
		dwResult = WaitForMultipleObjects(3, hObjects, FALSE, INFINITE);
		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			{
				bRun = false;
				break;
			}
		case WAIT_OBJECT_0 + 1:
			{
				pThreadMgr->DecreaseAvailNumber();
				if (pTaskItem = pTaskMgr->PopFront())
				{
					assert(pTaskItem->_pFunc);
					if (pTaskItem->_pFunc)
						pTaskItem->_pFunc(pTaskItem->_pParam);
					pTaskMgr->PushToIdle(pTaskItem);
				}
				else
				{
					assert(0);
				}
				pThreadMgr->IncreaseAvailNumber();
				break;
			}
		case WAIT_OBJECT_0 + 2:
			{
				bRun = false;
				pThreadMgr->DestroyThread(pThreadData->_uThreadId);
				break;
			}
		}
	}
	return 0;
}

unsigned int WINAPI ThreadPool::ThreadFunction(void* pContext)
{
	PTHREAD_ITEM pThreadData = static_cast<PTHREAD_ITEM>(pContext);
	ThreadPool *pThreadPool = pThreadData->_pThreadPool;
	
	UINT uResult = 0;
#ifdef _DEBUG
	uResult = pThreadPool->Execute(pThreadData);
#else
	__try
	{
		uResult = pThreadPool->Execute(pThreadData);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#endif
	return uResult;
}
