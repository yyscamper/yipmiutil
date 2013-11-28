@echo off
REM # buildwin.cmd
REM #   build ipmiutil for windows
REM #
REM # First download a copy of getopt.c getopt.h.
REM #   copy getopt.*   util
REM # Then download and build a copy of openssl for Windows,
REM # and copy the built openssl files needed to lib & inc.
REM #   copy libeay32.lib ssleay32.lib  lib
REM #   copy libeay32.dll ssleay32.dll  util
REM #   copy openssl\*.h   lib\lanplus\inc\openssl
REM #
REM # You should either run this from the Visual Studio Command Line, 
REM # or first run the appropriate vcvars.bat script.
set | findstr VCINSTALLDIR
if %errorlevel% EQU 1 goto vcerror

REM TODO, prebuild checking:
REM check for getopt.c,h
REM check for ssl libs
REM check for ssl includes
REM call mkssl

set UTMAKE=ipmiutil.mak
echo %LIBPATH% |findstr /C:amd64 >NUL
if %errorlevel% EQU 0  set UTMAKE=ipmiutil64.mak

cd lib
nmake /nologo -f ipmilib.mak all
cd ..
REM # echo make lib done

cd util
nmake /nologo -f %UTMAKE% all
cd ..
REM # echo make util done

echo buildwin ipmiutil done
goto done

:vcerror
echo First need to run vcvars.bat

:done

