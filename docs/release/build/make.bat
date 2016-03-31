del *.sym
del messui.exe
call make32 -j4 "OSD=messui" %1 %2 %3
if not exist messui.exe goto end
del mess.exe
call make32 -j4 "OSD=newui" %1 %2 %3
:end
