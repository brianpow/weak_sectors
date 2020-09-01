#!/bin/sh
echo
echo creating test file to verify 2-sheep-writer....
echo
read -p "Press ENTER to continue."
weak_sectors -create -list=sd251+ -out=sd251.dat -dup=10
echo
echo Use Nero to write sd251.dat as a plain data disc.
echo If a Toshiba SD-M 1502 can then read this disc, then you
echo have a 2-sheep writer.
echo
read -p "Press ENTER to continue."