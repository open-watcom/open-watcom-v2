#pmake: build os_qnx cpu_386 386 intel
#
# WATCOM Debugger makefile for QNX
#

host_OS=qnx
host_CPU=386
display=char
extra_cflags=-ms

sysobjs =   &
        qnxinit.obj &
        qnxlkup.obj &
        qnxchk.obj &
        qnxcmd.obj &
        qnxfilio.obj &
        qnxscrn.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) sys qnx386 flat
    @%append $(lnk) opt stack = 0x8000, offset=0x9000, resource=wvqnx.u
