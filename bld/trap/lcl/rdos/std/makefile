#pmake: build lcl os_rdos cpu_386 trap x86

tree_depth = 5

host_os  = rdos
host_cpu = 386

target_cpu = x86

#
# Create the RDOS STD.DLL
#

srv     = lcl
which = TRAP

extra_objs = &
        x86cpu.obj &
        task.obj &
        globals.obj &
        debug.obj &
        accmisc.obj &
        accload.obj &
        accio.obj &
        accfinfo.obj &
        accenv.obj &
        accbrwat.obj &
        accrun.obj &
        accasync.obj &
        accregs.obj &
        accmap.obj &
        accredir.obj &
        accmem.obj &
        accrtrd.obj &
        supphdlr.obj &
        supp.obj &
        core.obj

extra_incs = -I"$(clib_dir)/h"

!include $(trap_dir)/lcl/rdos/rdos.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
