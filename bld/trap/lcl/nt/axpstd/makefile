#pmake: lcl nt 32 trap axp
#
# Create the NT STD.DLL for Alpha
#

os      = nt
srv     = lcl
cpu     = axp

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

extra_incs = $(trap_dir)\lcl\dos\h
extra_cflags =

!include $(trap_dir)\lcl\lcl.mif
!include $(trap_dir)\trap.mif

globals.obj :: ..\h\globals.h

