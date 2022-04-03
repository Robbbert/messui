del mess64.sym
:start1
@del mess64.exe
@if exist mess64.exe goto start1
call make64 -j4 "OSD=newui" %1 %2 %3
:end
