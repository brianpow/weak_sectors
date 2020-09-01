# Introduction

This program is to experiment with weak sectors, which have been introduced with the SafeDisc 2 - copy protection.

It can create weak sectors as well as scan for them.

# Compile

## Linux
```
autoreconf -i
./configure
make
```

# Scan for weak sectors

Call:
```
weak_sectors -sector-size=n -in=filename
```

For CloneCD images, the sector size is always n = 2352.

You can also enter nothing for filename . Then, the source file name will be read from stdin, meaning that it can be provided through a pipe.
Example: You want to scan the only IMG file which is in the same directory as this tool. This could be done using the following call (e.g. stored in a BAT file):

```
dir /b *.img | weak_sectors -in= -sector-size=2352
```

You would receive the following output ("Star Trek - Bridge Commander"):

source file:STBC.img
```
Sector   1034: Occurence of pattern 5859 (0/0/  0): 1024 times =>  
Sector   1035: Occurence of pattern 5859 (0/0/  0): 1024 times =>  
...
Sector   1120: Occurence of pattern 7D65 (1/1/  0): 1024 times => w
Sector   1121: Occurence of pattern 7D65 (1/1/  0): 1024 times => w
Sector   1122: Occurence of pattern 7D65 (1/1/  0): 1024 times => w
Sector   1123: Occurence of pattern 7D65 (1/1/  0): 1024 times => w
Sector   1154: Occurence of pattern 14B9 (1/2/-14): 1024 times => w
Sector   1185: Occurence of pattern 082A (0/0/  0): 1024 times =>  
...
Sector   1264: Occurence of pattern 3F65 (1/1/  0): 1024 times => w
Sector   1265: Occurence of pattern 3F65 (1/1/  0): 1024 times => w
Sector   1296: Occurence of pattern 1212 (0/2/-14): 1024 times => w
Sector   1327: Occurence of pattern 2D9E (0/1/  0): 1024 times =>  
MBytes done:  757
```

Meaning:

   - number of the suspicious sector, with the first sector being number zero
   - patterns which have been found often in that sector, and how often. 1024 is the maximum ("regular pattern"), while 30 and more is consideres suspicious
   - "w" => pattern is possibly weak
   - " " => pattern is most likely not weak


The following weak patterns are currently in use:

   - SafeDisc 2: 3F65, 7D65
   - SafeDisc 2.51: 1212, 14B9

SCAN.BAT:
=> scans the first IMG file it finds in the current directory and saves the result to result.txt

# Creating test files

This call is a bit more complicated, that's why some files are already prepared to execute typical calls.
Call:
```
weak_sectors -create -forcemin=n -forcemax=n -forceminsum=n -forcemaxsum=n -audio -minDSV=n -dup=n -gap=n -out=file name -list=list_name
```

The following ones are important for you:

```
  -create 	activates test file creation mode
  -dup 	specifies the number of copies of each weak patterns which is stored in the output file
  -gap 	specifies the number of non-weak sectors which is added after a sequence of weak patterns
  -out 	specifies the name of the output file
  -audio 	writes unscrambled data into the output file. The files is then to be burned as audio, e.g. using cdrecord
  -list 	allows to create typical weak sector test files. Possible is sd2old and sd251+
```

The following files are prepared:
```
sd2old.bat 	creates a test file containg all regular patterns which are on discs up to SafeDisc 2.41
sd251.bat 	creates a test file containg all regular patterns which are on discs with SafeDisc 2.51 and higher
sd290.bat 	creates a test file containg all regular patterns which are on discs with SafeDisc 2.90 and higher
bad.bat 	creates a test file containg all very bad regular patterns
sheep_3.bat 	creates a test file containg all patterns which could be weak
sheep_3_audio.bat 	same as sheep_3.bat, but creates an audio file.
```
Write each test file to a CD-R or CD-RW disc using Mode 1 data format and try to read it back in different drives. A Toshiba DVD-ROM should be among them.

# Author
https://www.alexander-noe.com/ 
