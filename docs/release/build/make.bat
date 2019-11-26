@del messui64.sym
:start
del messui64.exe
@if exist messui64.exe goto start
call make64 -j4 "OSD=messui" %1 %2 %3
if not exist messui64.exe goto end
copy /Y messui64.exe messui.exe
copy /Y messui64.sym messui.sym
@del mess64.sym
:start1
del mess64.exe
@if exist mess64.exe goto start1
call make64 -j4 "OSD=newui" %1 %2 %3
if not exist mess64.exe goto end
@copy /Y mess64.exe mess.exe
@copy /Y mess64.sym mess.sym
mess64 -validate
:end
