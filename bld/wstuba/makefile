#pmake: build os_dos os_win cpu_i86 cpu_386

host_os  = dos
host_cpu = i86

proj_name = wstuba
wstuba_autodepends = .autodepend

!include cproj.mif
!include defrule.mif
!include deftarg.mif

all : wstub.exe wstubq.exe

wstub.exe : wstub.obj
        $(linker) $(lflags) file $[@

wstubq.exe : wstubq.obj
        $(linker) $(lflags) file $[@
