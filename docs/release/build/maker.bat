@del mess64.sym
:start1
@del mess64.exe
@if exist mess64.exe goto start1
@touch src\mame\mess.flt
call make64 -j6 "ARCHOPTS='-fuse-ld=lld'" "OSD=newui" %1 %2 %3
if not exist mess64.exe goto end
@copy /Y mess64.exe mess.exe
@copy /Y mess64.sym mess.sym
rem @call v.bat
:end