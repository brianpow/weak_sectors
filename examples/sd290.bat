@echo off
echo.
echo creating test file for SaveDisc 2.90....
echo.
pause
weak_sectors -create -list=sd290+ -out=sd290.dat -dup=10
echo.
echo Use Nero to write sd290.dat as a plain data disc.
echo If a Toshiba SD-M 1502 can then read this disc, then you
echo have a 2-sheep writer.
echo.
pause