#
#  Makefile for   res2wres
#
#   Date        Name        Reason
#   ----        ----        ------
#   7 June 91   Steve B.    created

#which memory model to use
MODEL = s       #small

# path to files
TRMEM_DIR = $(utils_root)\trmem
wresdir =   $(build_root)\sdk\rc\wres\

# name the compiler and linker
CC = wccp
LINK = wlinkp

# options to use
CFLAGS = -d1 -m$(MODEL) -fpc -oxt -zq -w4
TRMEM = -dTRMEM -of+

# where to look for various files
.c : ..\c;$(TRMEM_DIR)
.h : ..\h;$(TRMEM_DIR)

!include objlist.mif

# erase files on error
.ERASE

.BEFORE
        @set INCLUDE=$(watcom_h);$(TRMEM_DIR);$(wresdir)h;$(%include);

# explicit rules

all : res2res.exe .SYMBOLIC
        @%null

objlist : .SYMBOLIC
        @echo ******** Making objlist ********
        @ls ..\c\*.c >objlist.tmp
        @echo OBJS = &>objlist.mif
        @gres '..\\c\\([a-zA-Z0-9]*)\.c' '      \1.obj \&' objlist.tmp >>objlist.mif
        @gres '..\\c\\([a-zA-Z0-9]*)\.c' 'file \1' objlist.tmp >objlist.lnk
        @echo   trmem.obj &>>objlist.mif
        @echo   trmemcvr.obj &>>objlist.mif
        @echo file trmem >>objlist.lnk
        @echo file trmemcvr >>objlist.lnk
        @rm objlist.tmp

res2res.exe : $(OBJS) $(wresdir)lib\wres$(MODEL).lib
        $(LINK) libpath $(wresdir)lib option quiet @$^&.lnk

clean : .SYMBOLIC
    rm *.obj
    rm res2res.exe

trmemcvr.obj : trmemcvr.c
        $(CC) $[* $(CFLAGS) $(TRMEM)

# implicit rules

.c.obj :
        $(CC) $[* $(CFLAGS)
