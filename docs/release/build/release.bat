@echo off
echo HAVE YOU UPDATED version.cpp ???
pause


call newsrc.bat
call clean.bat
call clean.bat
call clean.bat
call clean.bat

rem --- 32bit ---
del messui.exe
del messui.sym
call make32 -j4 "OSD=messui" %1 %2 %3
if not exist messui.exe goto end
del mess.exe
del mess.sym
call make32 -j4 "OSD=newui" %1 %2 %3
if not exist mess.exe goto end

rem --- 64bit ---
del messui64.exe
del messui64.sym
call make64 -j4 "OSD=messui" %1 %2 %3
if not exist messui64.exe goto end
del mess64.exe
del mess64.sym
call make64 -j4  "OSD=newui" %1 %2 %3
if not exist mess64.exe goto end

cls
echo Compile was successful.
echo.
echo RAR up mess and messui;
echo RAR up mess64 and messui64;
echo rename mameui to messui and then RAR it;
echo rename mameui64 to messui64 and then RAR it;
echo each RAR must include license stuff.

:end
