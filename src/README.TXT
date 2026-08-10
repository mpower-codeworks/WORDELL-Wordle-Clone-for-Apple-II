    / / / / / / / /
  / / WORDELL / /
/ / / / / / / /

      2026
     mpower

Files needed on the Apple II disk:

  WORDELL.SYSTEM
  ALL5.BIN
  SOL5.BIN

Source files:
------------

  WORDELL.c          main game  and  low-level
                     fixed-position file reads

  FASTWORD.s         6502 RAM  search routine

  WORDDATA.h         generated offsets/counts
                     for ALL5.BIN

  MAKEPACK.C         optional PC-side packer
                     for rebuilding the data
                     files

  BUILD_WORDELL.bat  cc65 build command

The packed files  are generated from  ALL.TXT
and SOLUTION.TXT.  They contain  only  5-byte
words back-to-back. There are  no CR, LF,  or
zero terminators.

The largest letter group is S:

  1560 words * 5 bytes = 7800 bytes

So  the  program  only  needs  one  7800-byte
validation  buffer  instead  of  loading  the
whole 64K-ish ALL5.BIN file into RAM.
