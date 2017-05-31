//goto start0
del messui.sym
:start
@del messui.exe
@if exist messui.exe goto start
call make32 -j4 "OSD=messui" %1 %2 %3
if not exist messui.exe goto end
:start0
del mess.sym
:start1
@del mess.exe
@if exist mess.exe goto start1
call make32 -j4 "OSD=newui" %1 %2 %3
:end
