#pragma once

/**
 ** ���̲����������
 ** designed by eric,2016/08/23
 **/
class CProcHelper
{
public:
    /**
     ** ���ý��̵������У������������ͬ����EXE
     ** @param[out] procName            ������
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
    static bool SetSigletonProc(const char *pszProcName);
};
	