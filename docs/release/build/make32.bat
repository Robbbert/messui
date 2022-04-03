@echo off
set MINGW32=E:\Mingw\5-3-0\mingw32
set minpath=%MINGW32%\bin
set oldpath=%Path%
set Path=%minpath%;%oldpath%
%MINGW32%\bin\make SUBTARGET=mess PTR64=0 SYMBOLS=1 SYMLEVEL=1 STRIP_SYMBOLS=1 DEPRECATED=0 %1 %2 %3 %4
set Path=%oldpath%
set oldpath=
if exist mess.exe %minpath%\strip -s mess.exe
if exist messui.exe %minpath%\strip -s messui.exe
set minpath=

