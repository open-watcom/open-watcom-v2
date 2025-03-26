#pmake: build trap remote os_os2 cpu_386 par

tree_depth = 4

host_os  = os2
host_cpu = i86

proj_name = os2v2pio

os2v2pio_autodepends = .AUTODEPEND

os2v2pio_nocheckstack = 1

# This is a quick and dirty project because it is OS/2 specific and
# intended to be temporary; Yes, I know it breaks all known rules. Once
# WLINK properly supports 16-bit IOPL segments in 32-bit DLLs, the port
# I/O can be built into the parallel trap file/server modules, which is
# where it belongs in the first place.

!include cproj.mif
!include defrule.mif
!include deftarg.mif

.c : ../c

inc_dirs = -I"../h"

extra_c_flags_f_wdio = -nt=port -nc=IOPL_CODE -zls

wdio.dll : wdio.obj
    $(noecho)%create $^&.lnk
    @%append $^&.lnk format os2 dll option osname='OS/2 16-bit DLL'
    @%append $^&.lnk op map, elim, protmode, nodef
    @%append $^&.lnk name $@
    @%append $^&.lnk file wdio.obj
    @%append $^&.lnk segment 'port' IOPL 
    @%append $^&.lnk export INPORT 2
    @%append $^&.lnk export OUTPORT 4
    @%make echo_link
    $(linker) @$^&.lnk
