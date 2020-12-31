@echo off
@del mess64.sym
:start1
del build\generated\resource\messvers.rc
@del mess64.exe
@if exist mess64.exe goto start1
@touch src\mame\mess.flt
copy /Y src\osd\winui\uicl.rc src\osd\winui\mameui.rc
call make64 -j6 "ARCHOPTS='-fuse-ld=lld'" "OSD=newui" %1 %2 %3
if not exist mess64.exe goto end
del mess.exe
del mess.sym
@copy mess64.exe mess.exe
@copy mess64.sym mess.sym
rem @call v.bat
:end
