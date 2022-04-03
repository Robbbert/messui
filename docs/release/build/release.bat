@echo off
echo HAVE YOU UPDATED version.cpp ???
pause


call newsrc.bat
call clean.bat
call clean.bat
call clean.bat
call clean.bat

rem --- 64bit ---
call makee.bat
if not exist messui.exe goto end
call maker.bat
if not exist mess.exe goto end

cls
echo Compile was successful.
echo.

:end
