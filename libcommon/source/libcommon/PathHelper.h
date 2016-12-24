#pragma once
#include "Singleton.h"
#include <ShlObj.h>

#define REG_KEY_DISPLAY_VERSION     L"DisplayVersion"
#define REG_KEY_UNINSTALL_STRING    L"UninstallString"
#define REG_KEY_DISPLAY_ICON        L"DisplayIcon"
#define REG_KEY_INST_LOCATION       L"InstallLocation"
#define REG_ITEM_SHELL_FOLDER		L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
#define REG_ITEM_UNINSTALL_32BIT	L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
#define REG_ITEM_UNINSTALL_64BIT	L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"

/**
 ** ·��������
 ** designed by eric,2016/11/15
 **/
namespace eric { namespace common {
class PathHelper : public CSingleton<PathHelper>
{
	friend class CSingleton<PathHelper>;
public:
	/**
     ** ��ȡLocal AppData·��
	 ** Win7 C:\Users\�û���\AppData\Local
	 ** WinXP C:\Documents and Settings\�û���\Local Settings\Application Data
     ** @return                 Local·��
     **/
    std::wstring GetLocalPath();
    /**
     ** ��ȡLocalLow·��
	 ** Win7 C:\Users\lscm\AppData\LocalLow
	 ** WinXP ������
     ** @return                 LocalLow·��
     **/
    std::wstring GetLocalLowPath();
    /**
     ** ��ȡRoaming·��
	 ** Win7 C:\Users\�û���\AppData\Roaming
	 ** WinXP C:\Documents and Settings\�û���\Application Data
     ** @return                 Roaming·��
     **/
    std::wstring GetRoamingPath();
    /**
     ** �ӻ��������л�ȡAllUserAppData·��,�п���ʧ��
	 ** Win7 C:\ProgramData
	 ** WinXP C:\Documents and Settings\All Users
     ** @return                 AllUserAppData·��
     **/
    std::wstring GetAllUserAppDataPath();
    /**
     ** ��ȡDesktop·��
     ** @return                 Desktop·��
     **/
	std::wstring GetDesktopPath();
	/**
     ** ��ȡStartmenu·��
     ** @return                 Startmenu·��
     **/
    std::wstring GetStartmenuPath();
	/**
     ** ��ȡ��ʱĿ¼tmp·��
     ** Win7 C:\Users\�û���\AppData\Local\Temp
     ** WinXP C:\Documents and Settings\�û���\Local Settings\Temp
     ** @return                 ��ʱĿ¼��·��
     **/
    std::wstring GetTemporaryPath();    
    /**
     ** ��ȡProgram Files��·��
     ** @return                 Program Files��·��
     **/
    std::wstring GetProgramFilesPath();
    /**
     ** ��ȡProgram Files(X86)��·��
	 ** Win7 C:\Program Files (x86)
	 ** WinXP C:\Program Files
     ** @return  Program Files(X86)��·��
     **/
    std::wstring GetProgramFilesX86Path();
    /**
     ** ��ȡϵͳ�̷�
     ** @return                 ϵͳ�̷��ַ���
     **/
    std::wstring GetSystemDriver();
	/**
     ** ��ע����л�ȡ·��
     ** @param[in] hKey         ע����ֵ�����磺HKEY_CURRENT_USER
     ** @param[in] subKey       ע����������磺Software\Microsoft\Windows\CurrentVersion\Uninstall\365����
     ** @param[in] name         ��Ҫ���ҵļ�ֵ�����磺DisplayIcon
	 ** @param[in] boSamDesired	ע���Wow6432Node����ͨ�ģ�����������ֻ��һ������Ӧ��RegReadValue��samDesired�
     ** @return                 ��װĿ¼��·�������磺C:\Users\xiaof\AppData\Local\365Weather\Application\Default\365Weather.exe
     **/
    std::wstring GetPathFromReg(const HKEY& hKey, const std::wstring& subKey, const std::wstring& name, bool boSamDesired = false);

    /**
     ** ��CSIDL��ȡ·�������磺CSIDL_LOCAL_APPDATA��ʾLocal·��
     ** @param[in] csidl        CSIDLֵ
     ** @return                 ·���ַ���
     **/
    std::wstring GetPathFromCSIDL(const int csidl);
    /**
     ** �ӻ��������л�ȡ·��
     ** @param[in] name         �������������֣����磺ALLUSERSPROFILE��ʾProgram Data��·��
     ** @param[in] subDir       ���ļ���
     ** @return                 ·���ַ���
     **/
    std::wstring GetPathFromEvr(const std::wstring& name, const std::wstring& subDir);
    /**
     ** �ӻ��������л�ȡ·��
     ** @param[in] strEvir      �������������֣����磺%ALLUSERSPROFILE%��ʾProgram Data��·��
     ** @return                 ·���ַ���
     **/
    std::string GetPathFromEvr(const char* strEvir);
	/**
     ** ��ϵͳ�л�ȡ��ע��ΪKnown Folders���ļ���·��
     ** @return                 �ļ���·��
     **/
    std::wstring GetKnownFolderPath(const KNOWNFOLDERID& kfid);
private:
	PathHelper();
    ~PathHelper();

private:
    std::wstring m_strLocal;         // Local·��
    std::wstring m_strLocalLow;      // LocalLow·��
    std::wstring m_strRoaming;       // Roaming·��
    std::wstring m_strAllUser;       // ProgramData·��
    std::wstring m_strDesktop;       // Desktop·��
    std::wstring m_strStartmenu;     // Startmenu·��
    std::wstring m_strTemprary;      // Temp·��
    std::wstring m_strProgramFiles;  // ProgramFiles·��
    std::wstring m_strInstall;       // ����İ�װ·��
    std::wstring m_strConfig;        // �����ļ�·��
    std::wstring m_strUserName;      // �û���
    bool    m_isSysProc;        // �Ƿ�Ϊ�û�����
};

}}
