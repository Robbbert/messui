del messui64.sym
:start
@del messui64.exe
@if exist messui64.exe goto start
call make64 -j4 "OSD=messui" %1 %2 %3
if not exist messui64.exe goto end
copy /Y messui64.exe messui.exe
:end
