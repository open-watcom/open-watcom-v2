#pmake: os_dos os_win cpu_i86 cpu_386

host_os  = dos
host_cpu = i86

!include cproj.mif
!include defrule.mif
!include deftarg.mif

#
# make stubs for real-mode compilers and tools
#
all: wcc386p.exe wccx.exe wfcp.exe wfc386p.exe wlinkp.exe

wcc386p.exe: execpgm.c
        $(cc) $[@ -ox -dPGM_NAME="wcc386.exe" -fo=.obj
        $(linker) system dos name $^@ file $[&.obj
wfc386p.exe: execpgm.c
        $(cc) $[@ -ox -dPGM_NAME="wfc386.exe" -fo=.obj
        $(linker) system dos name $^@ file $[&.obj
wccx.exe: execpgm.c
        $(cc) $[@ -ox -dPGM_NAME="wcc.exe" -fo=.obj
        $(linker) system dos name $^@ file $[&.obj
wfcp.exe: execpgm.c
        $(cc) $[@ -ox -dPGM_NAME="wfc.exe" -fo=.obj
        $(linker) system dos name $^@ file $[&.obj
wlinkp.exe: execpgm.c
        $(cc) $[@ -ox -dPGM_NAME="wlink.exe" -fo=.obj
        $(linker) system dos name $^@ file $[&.obj

