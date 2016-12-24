@echo off

set g_vs2010_path=%VS100COMNTOOLS%

set g_devenv_path="%g_vs2010_path%..\IDE\devenv.com"
set g_sln_path=libcommon.sln

del build.log


echo ----- begin build libcommon_x64d -----
%g_devenv_path% /rebuild "Debug|x64" /project libcommon %g_sln_path%  >>build.log
if %errorlevel% NEQ 0 goto :build_failed
echo ----- end build libcommon_x64d -----

echo ----- begin build libcommon_x64 -----
%g_devenv_path% /rebuild "Release|x64" /project libcommon %g_sln_path%  >>build.log
if %errorlevel% NEQ 0 goto :build_failed
echo ----- end build libcommon_x64 -----

echo ----- begin build libcommon_x64mtd -----
%g_devenv_path% /rebuild "Debug_MT|x64" /project libcommon %g_sln_path%  >>build.log
if %errorlevel% NEQ 0 goto :build_failed
echo ----- end build libcommon_x64mtd -----

echo ----- begin build libcommon_x64mt -----
%g_devenv_path% /rebuild "Release_MT|x64" /project libcommon %g_sln_path%  >>build.log
if %errorlevel% NEQ 0 goto :build_failed
echo ----- end build libcommon_x64mt -----


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