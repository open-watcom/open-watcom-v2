#pmake: os_nt cpu_axp axp
#
# WATCOM Debugger makefile for NT AXP (GUI mode)
#

host_CPU=axp
host_OS=nt
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
    @%append $(lnk) system ntaxp_win
    @%append $(lnk) option stack=64k
