#pmake: os_nt cpu_386
#
# WATCOM Debugger makefile for NT (real GUI version)
#

host_os=nt
host_cpu=386

sys_windowed = 1

dll = 1
extra_c_flags= -br

all : dve.exe .SYMBOLIC

sysobjs = ntstub.obj ntfilio.obj winlkup.obj winchk.obj ntcmd.obj ntfinfo.obj

!include ../../master.mif

dve.exe : $(resfile) stubdb.obj wd.lib $(__MAKEFILES__)
        $(linker) sys nt name $^@ debug all op symf,map file stubdb.obj lib wd.lib
        $(rc) -k $(resfile) dve.exe
