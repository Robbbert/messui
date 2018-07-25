:start0
del mess.sym
:start1
@del mess.exe
@if exist mess.exe goto start1
call make32 -j4 "OSD=newui" %1 %2 %3
:end
