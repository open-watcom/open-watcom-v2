#
#  Makefile for   rescmp
#
#   Date        Name            Reason
#   ----        ----            ------
#   25 June 97  David Huynh     created

# path names used in the script
build_root = d:\dev
utils_root = d:\utils

# name the compiler and linker
CC = $(comp386)
LINK = $(linker)

# options to use
CFLAGS = -bt=nt -d2

#--------------------------------------------------------------------------

.extensions:
.extensions: .exe .obj .c .h

.c : ..\c
.h : ..\h;$(watcom_h_dir)

.BEFORE
        @set INCLUDE=$(watcom_h_dir);$(%include)

#--------------------------------------------------------------------------

NAME = exedmp
LNK = $(name).lnk

FILES = $(DIR)main.$(EXT) &
        $(DIR)read.$(EXT) &
        $(DIR)format.$(EXT) &
        $(DIR)param.$(EXT) &

EXT = obj
DIR = .\
OBJS = $+ $(FILES) $-

EXT = h
DIR = ..\h\
HDRS = $+ $(FILES) $- &
        common.h &
        strings.h


#--------------------------------------------------------------------------

# explicit rules

$(name).exe : $(OBJS) $(HDRS) $(lnk)
        $(LINK) libpath $(wresdir)lib;c:\lang\lib286;c:\lang\lib286\dos;d:\dev\sdk\wde\win16\lib @$^&.lnk
        copy $(name).exe c:\bin

$(lnk) : makefile
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) system nt
        @%append $(lnk) name $(name).exe
        @%append $(lnk) option map
        @%append $(lnk) option symf
        @for %i in ($(objs)) do @%append $(lnk) file %i


clean : .SYMBOLIC
        del *.obj
        del $(name).exe

# implicit rules

.c.obj :
        $(CC) $[* $(CFLAGS)

.h.c :
        wtouch $[*

