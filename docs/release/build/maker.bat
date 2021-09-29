@echo off
@del mess.sym
del build\generated\resource\messvers.rc
:start1
@del mess.exe
@if exist mess.exe goto start1
@touch src\mame\mess.flt
copy /Y src\osd\winui\uicl.rc src\osd\winui\mameui.rc
call make64 -j4 "ARCHOPTS='-fuse-ld=lld'" "OSD=newui" %1 %2 %3
color
if not exist mess.exe goto end
@call v.bat
:end
