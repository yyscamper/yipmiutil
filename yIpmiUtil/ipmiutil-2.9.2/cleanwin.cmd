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

REM call cleanssl (?)

cd lib
nmake /nologo -f ipmilib.mak clean
cd ..
REM # echo make lib done

cd util
nmake /nologo -f ipmiutil.mak clean
del *.pdb 2>NUL
cd ..
REM # echo make util done

echo cleanwin ipmiutil done
