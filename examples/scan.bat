@echo off
dir /b *.img *.iso | weak_sectors -in= -sector-size=2352 > result.txt