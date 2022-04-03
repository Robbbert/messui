@echo off
set MINGW64=E:\Mingw\10-1-0\mingw64
set minpath=%MINGW64%\bin
set oldpath=%Path%
set Path=%minpath%;%oldpath%
echo.|time
%MINGW64%\bin\make SUBTARGET=mess PTR64=1 SYMBOLS=0 NO_SYMBOLS=1 DEPRECATED=0 %1 %2 %3 %4
echo.|time
set Path=%oldpath%
set oldpath=
if exist mess.exe %minpath%\strip -s mess.exe
if exist messui.exe %minpath%\strip -s messui.exe
set minpath=

