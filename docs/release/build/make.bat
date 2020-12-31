@if exist scripts\minimaws\minimaws.sqlite3 del scripts\minimaws\minimaws.sqlite3
call makee.bat %1 %2 %3 %4
if not exist messui64.exe goto end
call maker.bat %1 %2 %3 %4
if not exist mess64.exe goto end
:end
