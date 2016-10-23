#pragma once

/**
 ** 进程操作的相关类
 ** designed by eric,2016/08/23
 **/
class CProcHelper
{
public:
    /**
     ** 设置进程单例运行，避免启动多个同样的EXE
     ** @param[out] procName            进程名
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
    static bool SetSigletonProc(const char *pszProcName);
};
	