#pmake: build os_nt cpu_386
#
# WATCOM Debugger makefile for NT (character mode)
#

host_OS=nt
host_CPU=386
display=char

sysobjs =   &
    ntinit.obj &
    ntscrn.obj &
    ntfilio.obj &
    ntfinfo.obj &
    ntcmd.obj &
    winchk.obj &
    winlkup.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) system nt
    @%append $(lnk) option stack=30k
