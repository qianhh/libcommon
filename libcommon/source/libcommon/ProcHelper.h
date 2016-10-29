#pragma once

/**
 ** 进程操作的相关类
 ** designed by eric,2016/08/23
 **/
namespace eric { namespace common {
class CProcHelper
{
public:
    /**
     ** 设置进程单例运行，避免启动多个同样的EXE
     ** @param[in] pszProcName				进程名
     ** @retval								true 设置成功; false 设置失败，可能已存在该进程
     **/
    static bool SetSigletonProc(const wchar_t *pszProcName);
};

}}
