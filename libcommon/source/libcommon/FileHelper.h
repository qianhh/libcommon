#pragma once

/**
 ** 文件操作的相关类
 ** designed by eric,2016/11/14
 ** modified by eric,2016/11/24
 **/
namespace eric { namespace common {
class FileHelper
{
public:
	/**
     ** 判断给出路径是否是一个目录
     ** @param[in] strPath              文件夹的目标路径
	 **									如果目标路径不存在,请确保在路径后面加"\"来表示这是一个目录,盘符会当成目录对待
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	static bool IsDirPath(const std::wstring& strPath);
	/**
     ** 判断文件或者目录是否存在
     ** @param[in] strPath              文件的路径
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	static bool IsFileOrDirExist(const std::wstring& strFile);
	/**
     ** 创建一个新的文件夹,该函数会逐层创建不存在的目录
     ** @param[in] strPath              新的文件夹的全路径
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	static bool CreateDir(const std::wstring& strPath);
	/**
     ** 在Temp目录下生成一个随机名字的文件夹，以当前时间为随机数，保证名字的唯一性
     ** @return                         临时目录的路径
     **/
    static std::wstring GenerateTempRandomDir();
	/**
     ** 删除文件
     ** @param[in] strFile              文件的全路径
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	static bool DelFile(const std::wstring& strFile, bool delReadonly = true);
	/**
     ** 删除文件夹，允许undo，undo的的意思是指是否可以从回收站回收
     ** @param[in] strPath              文件夹的路径
     ** @param[in] bAllowUndo           是否支持undo
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	static bool DeleteDir(const std::wstring& strPath, bool bAllowUndo = false);
	/**
     ** 删除文件夹，如果文件被占用，会在电脑重启后删除(MOVEFILE_DELAY_UNTIL_REBOOT)
     ** @param[in]   strPath            文件夹的路径
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	static bool EmptyDir(const std::wstring& strPath);
	/**
     ** 删除文件或文件夹，如果文件被占用，会在电脑重启后删除(MOVEFILE_DELAY_UNTIL_REBOOT)
     ** @param[in]   strDstPath         文件或文件夹的路径
     ** @param[in]   bAllowUndo         是否允许Undo操作
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	static bool DeleteFileOrDir(const std::wstring& strDstPath, const bool bAllowUndo = false);
	/**
     ** 分解文件路径为目录和文件名
     ** @param[in] strFullPath          文件或者文件夹路径,如果是文件夹请确保最后一位是"\"
     ** @param[out] strDirectory        文件所在的路径，以"\"结尾
     ** @param[out] strFileName         文件名
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	static bool SplitDirectoryAndFileName(const std::wstring strFullPath, std::wstring& strDirectory, std::wstring& strFileName);
	/**
     ** 重启后删除被占用的文件
     ** @param[in]   strFile            文件的全路径
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	static bool RebootRemoveFile(const std::wstring& strFile);
	/**
     ** 创建一个文件
     ** @param[in] strFileName          文件路径
     ** @retval                         true 成功
     ** @retval                         false 失败
     **/
	static bool CreateFileAlways(const std::wstring &strFileName);
};

}}
