
rem --- 32bit ---
del messui.exe
call make32 -j4 "OSD=messui" %1 %2 %3
if not exist messui.exe goto end
del mess.exe
call make32 -j4 "OSD=newui" %1 %2 %3

rem --- 64bit ---
del messui64.exe
call make64 -j4 "OSD=messui" %1 %2 %3
if not exist messui64.exe goto end
del mess64.exe
call make64 -j4  "OSD=newui" %1 %2 %3

rem --- 32bit full ---
del mameui.exe
call make32a -j4 "OSD=messui" %1 %2 %3
if not exist mameui.exe goto end

rem --- 64bit full ---
del mameui64.exe
call make64a -j4 "OSD=messui" %1 %2 %3
if not exist mameui64.exe goto end

cls
echo Compile was successful.
echo.
echo RAR up mess and messui;
echo RAR up mess64 and messui64;
echo rename mameui to messui and then RAR it;
echo rename mameui64 to messui64 and then RAR it;
echo each RAR must include license stuff.

:end
