@echo off
echo HAVE YOU UPDATED version.cpp ???
pause


call newsrc.bat
call clean.bat
call clean.bat
call clean.bat
call clean.bat
goto 64bit

rem --- 32bit ---
del messui.exe
del messui.sym
call make32 -j4 "OSD=messui" %1 %2 %3
if not exist messui.exe goto end
del mess.exe
del mess.sym
call make32 -j4 "OSD=newui" %1 %2 %3
if not exist mess.exe goto end

:64bit
rem --- 64bit ---
call makee.bat
if not exist messui64.exe goto end
call maker.bat
if not exist mess64.exe goto end

cls
echo Compile was successful.
echo.

:end
