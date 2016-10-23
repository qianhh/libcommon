#pragma once
#include "Singleton.h"

#define LOG_FLAG			//��־����
#define WRITE_FILE_FLAG		//��־д�ļ�����


#ifndef STRINGIFY
#define STRINGIFY(x) #x
#endif

#ifndef TOSTRING
#define TOSTRING(x) STRINGIFY(x)
#endif

#ifndef __QINFO__
#ifdef FILE_INFO_FLAG
#define __QINFO__ "["__FILE__"("TOSTRING(__LINE__)")]["__FUNCTION__"] "
#else
#define __QINFO__ "["__FUNCTION__"] "
#endif
#endif

/**
 ** ����־��, �������̰߳�ȫ��Ĭ��ʹ��Ӧ�ó���Ŀ¼��Ϊ��־Ŀ¼
 ** designed by eric,2016/10/12
 ** modified by eric,2016/10/18
 **/
class Logger : public CSingleton<Logger>
{
	friend class CSingleton<Logger>;
private:
	Logger(void);
	virtual ~Logger(void);

public:
	/**
	 ** �Զ�����־Ŀ¼ ע�����MOD_FILE��ONLY_FILE��pathΪ��־·��
	 ** @param[in] path				��־Ŀ¼
	 **/
	void SetLogDirectory(const char *path);
	/**
	 ** д��־
	 ** @param[in] prefix			��־��׼ǰ׺��Ϣ
	 ** @param[in] fmt				��ʽ����־
	 **/
	void Write(const char *prefix, const char *fmt, ...);

	enum MOD_FILE
	{
		ONLY_FILE = 0,		//ʼ��һ���ļ�
		PERDAY_FILE,		//ÿ��һ���ļ�
		PERMONTH_FILE,		//ÿ��һ���ļ�
	};
	/**
	 ** ����ģʽ ע��Ĭ������type=ONLY_FILE reserve=false
	 ** @param[in] type				��־�ļ�������
	 ** @param[in] reserve			�Ƿ�����ǰ����־�ļ� ע��ONLY_FILEģʽ��reserveΪfalse��ô���ļ���С����10M��������
	 **/
	void SetMod(MOD_FILE type, bool reserve);
	/**
	 ** ����д��־�ļ���ʱ���� ע��Ĭ������8*1000ms
	 ** @param[in] interval			ʱ����(ms)
	 **/
	void SetIntervalToWriteFile(unsigned int interval);
	/**
	 ** ����д��־�ļ��Ļ����С ע��Ĭ������4*1024�ֽ�
	 ** @param[in] size				�����С(�ֽ�)
	 **/
	void SetCacheSize(unsigned int size);

private:
	unsigned int Execute(void);
	static unsigned int __stdcall ThreadFunction(void* pContext);
	void GenerateLogFileName(void);
	void DeleteOldLogFile(void);

private:
	struct Impl;
	Impl *m_pImpl;
};


#ifdef LOG_FLAG
#define LOG_TRACE(__FORMAT__, ...) Logger::GetInstance()->Write(__QINFO__, __FORMAT__, __VA_ARGS__)
#else
#define LOG_TRACE
#endif

