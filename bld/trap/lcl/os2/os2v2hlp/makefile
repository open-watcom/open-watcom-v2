#pmake: os_os2 lcl cpu_386 support
#
# Create OS2V2HLP.EXE
#
host_cpu = 386
host_os  = os2

proj_name = os2v2hlp

!ifndef os2v2hlp_autodepends
os2v2hlp_autodepends = .AUTODEPEND
!endif

!include cproj.mif
!include defrule.mif
!include deftarg.mif

.asm : ../asm

extra_l_flags = op dosseg, map imp DOS32WRITE DOSCALLS.282,DOS32DEBUG DOSCALLS.317

os2v2hlp.exe : os2v2hlp.obj
        $(linker) $(lflags) name $^@ file os2v2hlp.obj
                
