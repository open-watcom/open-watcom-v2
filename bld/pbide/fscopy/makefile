#pmake: build cpu_i86 os_dos
proj_name = fscopy

host_os  = dos
host_cpu = i86

memory_model_i86 = s

!ifndef fscopy_autodepends
fscopy_autodepends = .AUTODEPEND
!endif

!include cproj.mif
!include defrule.mif
!include deftarg.mif

extra_l_flags = op map

wfscopy.exe : fscopy.obj
        $(linker) $(lflags) name $^@ file $[@
