#pmake: os_dos cpu_i86 i86 intel
#
# WATCOM Debugger makefile for DOS on the FMR PC
#

host_OS=dos
host_CPU=i86
display=char
machine=fmr
EXTRA_CFLAGS = -wo-of-d_OVERLAYED_

sysobjs =   &
    $(sysodir)\fork.obj &
    $(sysodir)\dosstl.obj &
    $(sysodir)\doschk.obj &
    $(sysodir)\doscmd.obj &
    $(sysodir)\dosfilio.obj &
    $(sysodir)\doslkup.obj &
    $(sysodir)\dosmem.obj &
    $(sysodir)\$(arch)scrn.obj &
    $(sysodir)\dosgrow.obj &
    $(sysodir)\dosalloc.obj &
    $(sysodir)\dostrap.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) sys dos op dyn,area=0k,stack=3k
