#!/bin/sh
echo
echo creating test file to verify 1-sheep-writer....
echo
read -p "Press ENTER to continue."
weak_sectors -create -list=sd2old -out=sd2old.dat -dup=10
echo
echo Use Nero to write sd2old.dat as a plain data disc.
echo If a Toshiba SD-M 1502 can then read this disc, then you
echo have a 1-sheep writer.
echo
read -p "Press ENTER to continue."