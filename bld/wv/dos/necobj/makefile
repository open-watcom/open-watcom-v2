#pmake: os_dos cpu_i86 i86 intel
#
# WATCOM Debugger makefile for NEC
#

host_OS=dos
host_CPU=i86
machine = nec
EXTRA_CFLAGS = -wo-of /d_OVERLAYED_
ui_library = $(uidir)\nec\uilibl.lib
sysodir = $(mdir)\dos\necobj
syshdir = $(mdir)\dos\h

!ifeq d 1
EXTRA_CFLAGS += /dDEBUG_NEC
!endif

sysobjs =   &
    $(sysodir)\fork.obj &
    $(sysodir)\dosstl.obj &
    $(sysodir)\doschk.obj &
    $(sysodir)\doscmd.obj &
    $(sysodir)\dosfilio.obj &
    $(sysodir)\doslkup.obj &
    $(sysodir)\dosmem.obj &
    $(sysodir)\dosgrow.obj &
    $(sysodir)\dosalloc.obj &
    $(sysodir)\dostrap.obj &
    $(sysodir)\necscrn.obj

!include ..\..\makefile

syslnk : .PROCEDURE
    @%append $(lnk) sys dos op dyn,area=0k,stack=3k
