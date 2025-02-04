# Makefile for zlib
# OpenWatcom large model
# Last updated: 28-Dec-2005

# To use, do "wmake -f watcom_l.mak"

C_SOURCE =  adler32.c  compress.c crc32.c   deflate.c    &
	    gzclose.c  gzlib.c    gzread.c  gzwrite.c    &
            infback.c  inffast.c  inflate.c inftrees.c   &
            trees.c    uncompr.c  zutil.c

OBJS =      adler32.obj  compress.obj crc32.obj   deflate.obj    &
	    gzclose.obj  gzlib.obj    gzread.obj  gzwrite.obj    &
            infback.obj  inffast.obj  inflate.obj inftrees.obj   &
            trees.obj    uncompr.obj  zutil.obj

CC       = wcc
LINKER   = wcl
CFLAGS   = -zq -ml -s -bt=dos -oilrtfm -fr=nul -wx
ZLIB_LIB = zlib_l.lib

.C.OBJ:
        $(CC) $(CFLAGS) $[@

all: $(ZLIB_LIB)

$(ZLIB_LIB): $(OBJS)
	wlib -b -c $(ZLIB_LIB) -+adler32.obj  -+compress.obj -+crc32.obj
	wlib -b -c $(ZLIB_LIB) -+gzclose.obj  -+gzlib.obj    -+gzread.obj   -+gzwrite.obj
        wlib -b -c $(ZLIB_LIB) -+deflate.obj  -+infback.obj
        wlib -b -c $(ZLIB_LIB) -+inffast.obj  -+inflate.obj  -+inftrees.obj
        wlib -b -c $(ZLIB_LIB) -+trees.obj    -+uncompr.obj  -+zutil.obj

clean: .SYMBOLIC
          del *.obj
          del $(ZLIB_LIB)
          @echo Cleaning done
