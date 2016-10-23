@echo off

set g_vs2010_path=%VS100COMNTOOLS%

set g_devenv_path="%g_vs2010_path%..\IDE\devenv.com"
set g_sln_path=libcommon.sln

del build.log


echo ----- begin build libcommond -----
%g_devenv_path% /rebuild "Debug|Win32" /project libcommon %g_sln_path%  >>build.log
if %errorlevel% NEQ 0 goto :build_failed
echo ----- end build libcommond -----

echo ----- begin build libcommon -----
%g_devenv_path% /rebuild "Release|Win32" /project libcommon %g_sln_path%  >>build.log
if %errorlevel% NEQ 0 goto :build_failed
echo ----- end build libcommon -----

echo ----- begin build libcommon_mtd -----
%g_devenv_path% /rebuild "Debug_MT|Win32" /project libcommon %g_sln_path%  >>build.log
if %errorlevel% NEQ 0 goto :build_failed
echo ----- end build libcommon_mtd -----

echo ----- begin build libcommon_mt -----
%g_devenv_path% /rebuild "Release_MT|Win32" /project libcommon %g_sln_path%  >>build.log
if %errorlevel% NEQ 0 goto :build_failed
echo ----- end build libcommon_mt -----


goto :build_successed

:build_failed
echo --------------------------------------
echo 当前工程编译失败！
echo --------------------------------------
pause
goto :end

:build_successed
echo --------------------------------------
echo 所有工程编译成功！
echo --------------------------------------

:end