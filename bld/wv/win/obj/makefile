#pmake: build os_win cpu_i86
#
# WATCOM Debugger makefile for Windows
#

host_OS=win
host_CPU=i86
display=gui
extra_cflags = -zW -zc

sysobjs =   &
    wininit.obj &
    winchk.obj &
    wincmd.obj &
    winfilio.obj &
    winlkup.obj &
    wguiscrn.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) system windows memory font
    @%append $(lnk) library windows
    @%append $(lnk) option heap=10K, stack=15K, align=512, verbose
