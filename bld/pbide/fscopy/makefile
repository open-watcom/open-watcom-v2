#pmake: build guitool cpu_i86 os_dos
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

wfscopy.exe : fscopy.obj $(__MAKEFILES__)
    @%make echo_link
    $(linker) name $^@ $(lflags) file $[@
