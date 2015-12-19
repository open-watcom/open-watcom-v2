#pmake: build lcl os_nt cpu_386 trap x86

host_os  = nt
host_cpu = 386

target_cpu = x86

#
# Create the NT STD.DLL
#

os      = nt
srv     = lcl
which = TRAP

extra_objs = &
        x86cpu.obj &
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
        supphdlr.obj &
        core.obj

inc_dirs += -I"$(trap_dir)/lcl/dos/h" -I"$(lib_misc_dir)/h"
extra_cpp_flags = -DWOW

extra_c_flags_accio = -I"$(lib_misc_dir)/h"

!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif

globals.obj :: ../h/globals.h

