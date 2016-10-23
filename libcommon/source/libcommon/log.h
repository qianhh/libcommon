#pragma once
#include "Singleton.h"

#define LOG_FLAG			//日志开关
#define WRITE_FILE_FLAG		//日志写文件开关


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
 ** 简单日志类, 单例，线程安全，默认使用应用程序目录作为日志目录
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
	 ** 自定义日志目录 注：如果MOD_FILE是ONLY_FILE则path为日志路径
	 ** @param[in] path				日志目录
	 **/
	void SetLogDirectory(const char *path);
	/**
	 ** 写日志
	 ** @param[in] prefix			日志标准前缀信息
	 ** @param[in] fmt				格式化日志
	 **/
	void Write(const char *prefix, const char *fmt, ...);

	enum MOD_FILE
	{
		ONLY_FILE = 0,		//始终一个文件
		PERDAY_FILE,		//每天一个文件
		PERMONTH_FILE,		//每月一个文件
	};
	/**
	 ** 设置模式 注：默认设置type=ONLY_FILE reserve=false
	 ** @param[in] type				日志文件的类型
	 ** @param[in] reserve			是否保留以前的日志文件 注：ONLY_FILE模式下reserve为false那么当文件大小超过10M会进行清空
	 **/
	void SetMod(MOD_FILE type, bool reserve);
	/**
	 ** 设置写日志文件的时间间隔 注：默认设置8*1000ms
	 ** @param[in] interval			时间间隔(ms)
	 **/
	void SetIntervalToWriteFile(unsigned int interval);
	/**
	 ** 设置写日志文件的缓存大小 注：默认设置4*1024字节
	 ** @param[in] size				缓存大小(字节)
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

