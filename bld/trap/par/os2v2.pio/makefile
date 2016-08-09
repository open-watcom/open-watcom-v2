#pmake: build trap remote os_os2 cpu_386 par

host_os  = os2
host_cpu = i86

proj_name = os2v2pio

os2v2pio_autodepends = .AUTODEPEND

# This is a quick and dirty project because it is OS/2 specific and
# intended to be temporary; Yes, I know it breaks all known rules. Once
# WLINK properly supports 16-bit IOPL segments in 32-bit DLLs, the port
# I/O can be built into the parallel trap file/server modules, which is
# where it belongs in the first place.

sys_dll = 1

!include cproj.mif
!include defrule.mif
!include deftarg.mif

!include ../os2io.mif

wdio.dll: wdio.obj $(__MAKEFILES__)
    $(linker) name $@ sys os2 dll op map,protmode file {$[@} $(extra_linker)
