#pmake: os_dos cpu_i86 i86 intel
#
# WATCOM Debugger makefile for DOS
#

nodebug=1
host_OS=dos
host_CPU=i86
EXTRA_CFLAGS = -wo-of-d_OVERLAYED_

sysobjs =   &
    fork.obj &
    dosstl.obj &
    doschk.obj &
    doscmd.obj &
    dosfilio.obj &
    doslkup.obj &
    dosmem.obj &
    dosgrow.obj &
    dosalloc.obj &
    dostrap.obj &
    dosscrn.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) sys dos
    @%append $(lnk) op stack=8k
    @%append $(lnk) op dyn,area=0k
