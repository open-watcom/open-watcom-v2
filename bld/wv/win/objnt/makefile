#pmake: build os_nt cpu_386
#
# WATCOM Debugger makefile for NT (real GUI version)
#

host_OS=nt
host_CPU=386
display=gui

sysobjs =   &
    ntinit.obj &
    ntguiscn.obj &
    ntfilio.obj &
    ntfinfo.obj &
    ntcmd.obj &
    winchk.obj &
    winlkup.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) system nt_win
    @%append $(lnk) option stack=64k
