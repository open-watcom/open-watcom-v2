#pmake: build os_dos cpu_386 386 intel
#
# WATCOM Debugger makefile for dsx
#

host_OS=dos
host_CPU=386
display=char
syshdir = ..\h;..\..\dos\h

sysobjs =   &
    dsxfork.obj &
    dsxstart.obj &
    dsxchk.obj &
    dsxcmd.obj &
    dsxfilio.obj &
    dsxscrn.obj &
    dsxlkup.obj &
    dsxhdlr.obj &
    dsxmem.obj


!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) sys dos4g op stack=12k
    @%append $(lnk) export DOS4GOPTIONS=_DOS4GOPTIONS
