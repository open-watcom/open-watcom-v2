#pmake: lcl os_nt cpu_386 trap axp
#
# Create the NT STD.DLL for Alpha
#
host_os  = nt
host_cpu = axp

os      = nt
srv     = lcl
cpu     = axp
which   = TRAP

extra_objs = &
        task.obj &
        globals.obj &
        accmisc.obj &
        accload.obj &
        accio.obj &
        accfinfo.obj &
        accenv.obj &
        accbrwat.obj &
        accrun.obj &
        accregs.obj &
        accmap.obj &
        accredir.obj &
        accmem.obj &
        misc.obj &
        thread.obj &
        peread.obj &
        supp.obj &
        dbgthrd.obj &
        core.obj

inc_dirs += -I"$(trap_dir)/lcl/dos/h" -I"$(lib_misc_dir)/h"
extra_c_flags =

extra_c_flags_accio = -I"$(lib_misc_dir)/h"

!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif

globals.obj :: ../h/globals.h
