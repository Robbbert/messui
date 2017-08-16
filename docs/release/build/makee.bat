del messui.sym
:start
@del messui.exe
@if exist messui.exe goto start
call make32 -j4 "OSD=messui" %1 %2 %3
