#pmake: os_nt cpu_386
#
# WATCOM Debugger makefile for NT (real GUI version)
#

host_OS=nt
host_cpu=386
display=gui
dll = 1
extra_cflags= -br

all : .symbolic
        @%make $(bdir)\dve.exe

lexusbld : .symbolic lib
    @newer wd.lib $(lexlib_dir)\wv\lib "^copy &f $#f > nul"
    @newer ..\..\h\*.* $(lexlib_dir)\wv\h "^copy &f $#f > nul"
    @newer $(dig_dir)\h\*.* $(lexlib_dir)\dig\h "^copy &f $#f > nul"

sysobjs = ntstub.obj ntfilio.obj winlkup.obj winchk.obj ntcmd.obj ntfinfo.obj

!include ..\..\makefile

stubdb.obj : ..\c\stubdb.c
         $(compile) /fh

$(bdir)\dve.exe : eraseall.obj lib stubdb.obj $(resfile) $(__MAKEFILES__)
        $(linker) debug all op symf name $(bdir)\dve.exe sys nt op map file stubdb.obj lib wd.lib
        *wrc -k -bt=nt -i=$(rcinclude) $(resfile) $(bdir)\dve.exe
