#pragma once

/**
 ** ���̲����������
 ** designed by eric,2016/08/23
 **/
namespace eric { namespace common {
class CProcHelper
{
public:
    /**
     ** ���ý��̵������У������������ͬ����EXE
     ** @param[in] pszProcName				������
     ** @retval								true ���óɹ�; false ����ʧ�ܣ������Ѵ��ڸý���
     **/
    static bool SetSigletonProc(const wchar_t *pszProcName);
};

}}
