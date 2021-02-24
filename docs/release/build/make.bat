@if exist scripts\minimaws\minimaws.sqlite3 del scripts\minimaws\minimaws.sqlite3
call makee.bat %1 %2 %3 %4
if not exist messui.exe goto end
call maker.bat %1 %2 %3 %4
if not exist mess.exe goto end
:end
