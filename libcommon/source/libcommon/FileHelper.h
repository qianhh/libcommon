#pragma once

/**
 ** �ļ������������
 ** designed by eric,2016/11/14
 ** modified by eric,2016/11/24
 **/
namespace eric { namespace common {
class FileHelper
{
public:
	/**
     ** �жϸ���·���Ƿ���һ��Ŀ¼
     ** @param[in] strPath              �ļ��е�Ŀ��·��
	 **									���Ŀ��·��������,��ȷ����·�������"\"����ʾ����һ��Ŀ¼,�̷��ᵱ��Ŀ¼�Դ�
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	static bool IsDirPath(const std::wstring& strPath);
	/**
     ** �ж��ļ�����Ŀ¼�Ƿ����
     ** @param[in] strPath              �ļ���·��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	static bool IsFileOrDirExist(const std::wstring& strFile);
	/**
     ** ����һ���µ��ļ���,�ú�������㴴�������ڵ�Ŀ¼
     ** @param[in] strPath              �µ��ļ��е�ȫ·��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	static bool CreateDir(const std::wstring& strPath);
	/**
     ** ��TempĿ¼������һ��������ֵ��ļ��У��Ե�ǰʱ��Ϊ���������֤���ֵ�Ψһ��
     ** @return                         ��ʱĿ¼��·��
     **/
    static std::wstring GenerateTempRandomDir();
	/**
     ** ɾ���ļ�
     ** @param[in] strFile              �ļ���ȫ·��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	static bool DelFile(const std::wstring& strFile, bool delReadonly = true);
	/**
     ** ɾ���ļ��У�����undo��undo�ĵ���˼��ָ�Ƿ���Դӻ���վ����
     ** @param[in] strPath              �ļ��е�·��
     ** @param[in] bAllowUndo           �Ƿ�֧��undo
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	static bool DeleteDir(const std::wstring& strPath, bool bAllowUndo = false);
	/**
     ** ɾ���ļ��У�����ļ���ռ�ã����ڵ���������ɾ��(MOVEFILE_DELAY_UNTIL_REBOOT)
     ** @param[in]   strPath            �ļ��е�·��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	static bool EmptyDir(const std::wstring& strPath);
	/**
     ** ɾ���ļ����ļ��У�����ļ���ռ�ã����ڵ���������ɾ��(MOVEFILE_DELAY_UNTIL_REBOOT)
     ** @param[in]   strDstPath         �ļ����ļ��е�·��
     ** @param[in]   bAllowUndo         �Ƿ�����Undo����
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	static bool DeleteFileOrDir(const std::wstring& strDstPath, const bool bAllowUndo = false);
	/**
     ** �ֽ��ļ�·��ΪĿ¼���ļ���
     ** @param[in] strFullPath          �ļ������ļ���·��,������ļ�����ȷ�����һλ��"\"
     ** @param[out] strDirectory        �ļ����ڵ�·������"\"��β
     ** @param[out] strFileName         �ļ���
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	static bool SplitDirectoryAndFileName(const std::wstring strFullPath, std::wstring& strDirectory, std::wstring& strFileName);
	/**
     ** ������ɾ����ռ�õ��ļ�
     ** @param[in]   strFile            �ļ���ȫ·��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	static bool RebootRemoveFile(const std::wstring& strFile);
	/**
     ** ����һ���ļ�
     ** @param[in] strFileName          �ļ�·��
     ** @retval                         true �ɹ�
     ** @retval                         false ʧ��
     **/
	static bool CreateFileAlways(const std::wstring &strFileName);
};

}}
