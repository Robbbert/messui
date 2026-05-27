copy /Y cover.bat c:\mess
robocopy c:\mess c:\coverity /MIR /NDL /XD build nvram ini cfg regtest
call clean.bat
call clean.bat
del build\generated\resource\messvers.rc
:start
del messui.exe
if exist messui.exe goto start
copy /Y src\mame\mess.flt src\mame\mess.bak
copy /Y src\mame\messui.txt src\mame\mess.flt
touch src\mame\mess.flt
touch src\version.cpp
copy /Y src\osd\winui\ui.rc src\osd\winui\mameui.rc
touch src\osd\winui\mameui.rc
set MINGW64=E:\Mingw\10-1-0\mingw64
set minpath=%MINGW64%\bin
set oldpath=%Path%
set Path=%minpath%;%oldpath%;e:\coverity\bin
cov-build --dir cov-int %MINGW64%\bin\make SUBTARGET=mess OSD=messui PTR64=1 -j4
set Path=%oldpath%
set oldpath=
set minpath=
copy /Y src\mame\mess.bak src\mame\mess.flt
color
