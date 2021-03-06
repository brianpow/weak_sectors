#!/bin/sh
echo
echo Creating test file to verify 3-sheep-writer....
echo This will consume 460 MB of free space.
echo
read -p "Press ENTER to continue."
weak_sectors -create -forceminsum=2 -dup=10 -gap=2 -mode2xa -out=sheep3_mode2xa.dat > out3xa.txt
echo
echo Use Nero to write sheep3.dat as a plain data disc.
echo If a Toshiba SD-M 1502 can then read this disc, then you
echo have a 3-sheep writer.
echo
read -p "Press ENTER to continue."