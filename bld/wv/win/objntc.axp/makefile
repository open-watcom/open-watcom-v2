#pmake: os_nt cpu_axp axp
#
# WATCOM Debugger makefile for NT (character mode)
#

host_CPU=axp
host_OS=nt
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
    @%append $(lnk) system ntaxp
    @%append $(lnk) option stack=64k
