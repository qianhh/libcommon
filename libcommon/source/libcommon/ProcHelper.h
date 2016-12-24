#pragma once

/**
 ** ���̲����������
 ** designed by eric,2016/08/23
 **/
namespace eric { namespace common {
class ProcHelper
{
public:
    /**
     ** ���ý��̵������У������������ͬ����EXE
     ** @param[in] pszProcName				������
     ** @retval								true ���óɹ�; false ����ʧ�ܣ������Ѵ��ڸý���
     **/
    static bool SetSigletonProc(const wchar_t *pszProcName);
	/**
     ** ��������
     ** @param[in] cmd          ������
     ** @param[in] appName      Ӧ�ó�������
     ** @param[in] workingDir   ��ǰ����·��
     ** @param[in] bShowWindow  �Ƿ���ʾ����
     ** @param[in] bInherit     �Ƿ�̳и�����
     ** @param[in] bWait        �Ƿ�֧���첽�ȴ�
     ** @param[in] dwWaitTime   �ȴ�ʱ��
     ** @param[in] pThreadID    �߳�ID
     ** @param[in] pProcessID   ����ID
     ** @param[in] pExitCode    �����˳���
     ** @retval                 true �ɹ�
     ** @retval                 false ʧ��
     **/
	static bool CreateProcessInherit(const std::wstring& cmd, 
									 const std::wstring& appName = L"",
									 const std::wstring& workingDir = L"",
									 const bool bShowWindow = false,
									 const bool bInherit = true,
									 const bool bWait = false,
									 DWORD dwWaitTime = 1000,
									 DWORD* pThreadID = NULL,
									 DWORD* pProcessID = NULL,
									 DWORD* pExitCode = NULL);
	/**
     ** �����û�����
     ** @param[in] path         ���̵�·��
     ** @param[in] cmd			������
     ** @param[in] bShowWnd		�Ƿ���ʾ���ڣ�true-��ʾ��false-����ʾ
     ** @retval                 true �ɹ�
     ** @retval                 false ʧ��
     **/
    static bool MyCreateProcessAsUser(const std::wstring& path, const std::wstring& cmd = L"", const bool bShowWnd = false);
	/**
     ** ��ȡ��ǰ���̵�ȫ·��
     ** @return								���̵�����
     **/
	static std::wstring GetCurrentProcessPathName(void);
	/**
     ** ��ȡ��ǰ�������ڵ�Ŀ¼
     ** @return								���̵�·��
     **/
    static std::wstring GetCurrentProcessPath(void);
	/**
     ** ��ȡ��ǰ���̵�����
     ** @return								���̵�����
     **/
	static std::wstring GetCurrentProcessName(void);
	 /**
     ** ��ȡ��ǰ������module
     ** @retval								moduleֵ
     **/
	static HMODULE GetCurrentModule(void);
	/**
     ** ��ȡ��ǰmodule��ȫ·��
     ** @return								module��·��
     **/
	static std::wstring GetCurrentModulePathName(void);
	/**
     ** ��ȡ��ǰmodule��Ŀ¼
     ** @return								module��·��
     **/
	static std::wstring GetCurrentModulePath(void);
	/**
     ** ��ȡ��ǰmodule������
     ** @return								module������
     **/
	static std::wstring GetCurrentModuleName(void);
	/**
     ** ע��DLL
     ** @param[in] dllPath					Ŀ��DLL��·��
     ** @retval								true �ɹ�
     ** @retval								false ʧ��
     **/
	static bool RegisterDLL(const std::wstring& dllPath, bool bSilent = true);
    /**
     ** ע��DLL
     ** @param[in] dllPath					Ŀ��DLL��·��
     ** @retval								true �ɹ�
     ** @retval								false ʧ��
     **/
	static bool UnregisterDLL(const std::wstring& dllPath, bool bSilent = true);
	/**
     ** ��ȡָ�����Ƶ�ģ��
     ** @param[in] modulePath				ģ�������
     ** @return								ָ��ģ��ı�־
     **/
	static HMODULE GetModule(const std::wstring& modulePath);
	/**
     ** ��ǰȨ����������
     ** @param[in] filePath					���̿�ִ���ļ�
	 ** @retval								true �ɹ�
	 ** @retval								false ʧ��
     **/
	static bool LaunchProcess(const std::wstring& filePath);
	/**
     ** ��userȨ����������
     ** @param[in] strCmdLine				�����У����磺C:\XX.exe Cmd=start
     ** @param[in] strFileName				���̿�ִ���ļ���Ĭ��Ϊ��
     ** @param[in] bShowWnd					�Ƿ���ʾ������棬Ĭ�ϲ���ʾ
	 ** @retval								true �ɹ�
	 ** @retval								false ʧ��
     **/
	static bool LaunchProcessAsUser(const std::wstring& strCmdLine, const std::wstring& strFileName = L"", bool bShowWnd = false);
	/**
     ** �жϵ�ǰ�����Ƿ��Ը������Լ������� 
	 ** @retval								true ��
	 ** @retval								false ��
     **/
	static bool IsProcessRunAsAdmin();
    /**
     ** �жϽ����Ƿ���Ϊϵͳ����(��SYSTEM�û������У�����Ϊ��ϵͳ����)
     ** @param[in] procID					����ID��Ĭ��ʹ�õ�ǰ����ID
	 ** @param[in] procName					�������֣�Ĭ��ʹ�õ�ǰ��������
     ** @retval								true ��
     ** @retval								false ��
     **/
    static bool IsSystemProcess(const DWORD& procID = 0);
	static bool IsSystemProcess(const std::wstring& procName = L"");
	/**
     ** �жϽ����Ƿ����
	 ** @param[in] procID					����ID
     ** @param[in] strProcName				��������
     ** @retval								true ����
     ** @retval								false ������
     **/
	static bool IsProcessExist(const DWORD& procID);
    static bool IsProcessExist(const std::wstring& strProcName);

    /**
     ** ��ȡ��ǰ���̵ĸ�������Ϣ��������������ID
     ** @param[out] parentProcName			������
     ** @param[out] procID					����ID
     ** @retval								true �ɹ�
     ** @retval								false ʧ��
     **/
    static bool GetParentProcessName(std::wstring& parentProcName, DWORD* procID = NULL);
	/**
     ** ��ȡ���н��̵��û����������Ҫ��ȡSYSTEM�û����������Թ���ԱȨ������
     ** @param[in] procID					����ID��Ĭ��ʹ�õ�ǰ����ID
     ** @return								�û����ַ���
     **/
    static std::wstring GetProcessUserName(const DWORD& procID = 0);
	/**
     ** ����Ȩ��(Ŀǰwin7ϵͳ��֧��Ȩ������)
     ** @param[in] privilege            Ȩ�޼������ƣ�������������(winnt.h�ļ���7812��)
	 ** @param[in] bEnablePrivilege     �Ƿ���������Ȩ��
	 ** @retval							true �ɹ�
	 ** @retval							false ʧ��
     **/
	static bool MySetPrivilege(const std::wstring& privilege, const bool bEnablePrivilege);
	/**
     ** ͨ������ID��ȡ������
     ** @param[in] dwProcID     ����ID
     ** @return                 ������
     **/
	static std::wstring GetProcessNameByPid(const DWORD dwProcID);
	/**
     ** ͨ����������ȡ����ID�����Win7���޷���ȡexplorer.exe��ID
     ** @param[in] procName     ������
     ** @param[in] strFlagStr   ·���е�һ����ʾ������ΪChrome�ͷ���������Ľ��̶���chrome.exe������ͨ���ñ�ʾ��������
     ** @return                 ����ID
     **/
    static DWORD GetProcessIDByName(const std::wstring& procName, const std::wstring& strFlagStr = L"");
};

}}
