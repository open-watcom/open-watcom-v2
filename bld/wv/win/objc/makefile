#pmake: build os_win cpu_i86
#
# WATCOM Debugger makefile for Windows
#

##
## NOTE: This file also gets used by "..\necobj\makefile"
##

host_OS=win
host_CPU=i86
display=char
extra_cflags += -zW -zc

sysobjs =   &
    wininit.obj &
    winchk.obj &
    wincmd.obj &
    winfilio.obj &
    winlkup.obj &
    winscrn.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) system windows memory font
    @%append $(lnk) library windows
    @%append $(lnk) option heap=15K, stack=12K
    @%append $(lnk) import DEATH GDI.121 import RESURRECTION GDI.122
