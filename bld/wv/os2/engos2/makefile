#pmake: os_os2 cpu_386
#
# Open Watcom Debugger engine makefile for OS/2
#

host_os  = os2
host_cpu = 386

sys_windowed = 1

dll = 1
extra_c_flags = -br

all : dve.exe .SYMBOLIC

sysobjs = os22flio.obj os2cmd.obj os22ini.obj os2chk.obj autoenv.obj

!include ../../master.mif

dve.exe : $(resfile) stubdb.obj wd.lib $(__MAKEFILES__)
        $(linker) sys os2v2 name $^@ debug all op symf,map file stubdb.obj lib wd.lib
        $(rc) $(resfile) dve.exe
