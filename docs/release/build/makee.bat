@echo off
del build\generated\resource\messvers.rc
:start
del messui.exe
if exist messui.exe goto start
del messui.sym
copy /Y src\mame\mess.flt src\mame\mess.bak
copy /Y src\mame\messui.flt src\mame\mess.flt
touch src\mame\mess.flt
touch src\version.cpp
copy /Y src\osd\winui\ui.rc src\osd\winui\mameui.rc
touch src\osd\winui\mameui.rc
call mk.bat
call make64 -j4 "OSD=messui" %1 %2 %3
rem call make64 -j4 "LDOPTS=-Wl,--pdb=" "OSD=messui" %1 %2 %3
rem call make64 -j4 "ARCHOPTS='-fuse-ld=lld'" "OSD=messui" %1 %2 %3
copy /Y src\mame\mess.bak src\mame\mess.flt
del /Q build\mingw-gcc\bin\x64\Release\mame_mess
if not exist messui.exe goto end
:end
