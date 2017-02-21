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
echo 7Z up mess and messui;
echo 7Z up mess64 and messui64;
echo each must include license stuff.

:end
