@echo off
set MINGW32=E:\Mingw\5-3-0\mingw32
set minpath=%MINGW32%\bin
set oldpath=%Path%
set Path=%minpath%;%oldpath%
%MINGW32%\bin\gdb --args mess %1 %2 %3
set Path=%oldpath%
set oldpath=
set minpath=

