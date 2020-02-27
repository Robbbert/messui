@echo off
del messui64.sym
:start
del messui64.exe
if exist messui64.exe goto start
copy /Y src\mame\mess.flt src\mame\mess.bak
copy /Y src\mame\messui.txt src\mame\mess.flt
touch src\mame\mess.flt
call make64 -j4 "OSD=messui" %1 %2 %3
copy /Y src\mame\mess.bak src\mame\mess.flt
if not exist messui64.exe goto end
copy /Y messui64.exe messui.exe
copy /Y messui64.sym messui.sym
:end
