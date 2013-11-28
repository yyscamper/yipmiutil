@echo off
REM # buildwin2.cmd
REM #   build ipmiutil for windows in standalone mode
REM #   without IPMI LAN 2.0 libraries, so no SOL.  Use for bootables.
REM #
REM # First download a copy of getopt.c getopt.h.
REM #   copy getopt.*   util
REM #

REM TODO, prebuild checking:
REM check for getopt.c,h

set | findstr VCINSTALLDIR
if %errorlevel% EQU 1 goto vcerror

set UTMAKE=ipmiutil2.mak
echo %LIBPATH% |findstr /C:amd64 >NUL
if %errorlevel% EQU 0  set UTMAKE=ipmiutil2-64.mak

cd util
nmake /nologo -f %UTMAKE% all
cd ..

echo buildwin2 ipmiutil done
goto done

:vcerror
echo Need to first run vcvars.bat

:done

