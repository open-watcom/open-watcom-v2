# Makefile for Open Watcom C/C++

.c : ..

CC = wcl386
CFLAGS = -oaxt -zq -6r

.c.obj: 
    $(CC) $(CFLAGS) -c $< -fo=$*.obj

OBJS= blocksort.obj  &
      huffman.obj    &
      crctable.obj   &
      randtable.obj  &
      compress.obj   &
      decompress.obj &
      bzlib.obj

all: libbz2.lib bzip2.exe bzip2recover.exe

bzip2.exe: libbz2.lib bzip2.obj
    $(CC) $(CFLAGS) -fm -fo=bzip2 bzip2.obj libbz2.lib #wildargv.obj

bzip2recover.exe: libbz2.lib bzip2recover.obj
    $(CC) $(CFLAGS) -fm -fo=bzip2recover bzip2recover.obj

libbz2.lib: $(OBJS)
    wlib -q libbz2.lib $(OBJS)

test: bzip2.exe .symbolic
    type ..\words1
    .\bzip2 -1  < ..\sample1.ref > sample1.rb2
    .\bzip2 -2  < ..\sample2.ref > sample2.rb2
    .\bzip2 -3  < ..\sample3.ref > sample3.rb2
    .\bzip2 -d  < ..\sample1.bz2 > sample1.tst
    .\bzip2 -d  < ..\sample2.bz2 > sample2.tst
    .\bzip2 -ds < ..\sample3.bz2 > sample3.tst
    comp ..\sample1.bz2 sample1.rb2 
    comp ..\sample2.bz2 sample2.rb2
    comp ..\sample3.bz2 sample3.rb2
    comp sample1.tst ..\sample1.ref
    comp sample2.tst ..\sample2.ref
    comp sample3.tst ..\sample3.ref

clean: .symbolic
    -del *.obj
    -del libbz2.lib 
    -del bzip2.exe
    -del bzip2recover.exe
    -del sample1.rb2 
    -del sample2.rb2 
    -del sample3.rb2
    -del sample1.tst 
    -del sample2.tst
    -del sample3.tst
