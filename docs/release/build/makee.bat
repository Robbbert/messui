@echo off
del build\generated\resource\messvers.rc
:start
del messui64.exe
if exist messui64.exe goto start
copy /Y src\mame\mess.flt src\mame\mess.bak
copy /Y src\mame\messui.txt src\mame\mess.flt
touch src\mame\mess.flt
touch src\version.cpp
copy /Y src\osd\winui\ui.rc src\osd\winui\mameui.rc
touch src\osd\winui\mameui.rc
call make64 -j6 "OSD=messui" %1 %2 %3
copy /Y src\mame\mess.bak src\mame\mess.flt
if not exist messui64.exe goto end
del messui.exe
copy messui64.exe messui.exe
del messui64.sym
:end
