@echo off
echo.
echo Creating test file with only very bad patterns....
echo.
pause
weak_sectors -create -forceminsum=4 -minDSV=60 -dup=10 -gap=2 -out=bad.dat > bad.txt
echo.
echo Use Nero to write bad.dat as a plain data disc.
echo If a Toshiba SD-M 1502 can then read this disc, then you
echo have a good writer, maybe 3 sheep.
echo.
pause