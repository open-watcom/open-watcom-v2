#pmake: build lcl nt 32 trap x86
#
# Create the NT STD.DLL
#

os      = nt
srv     = lcl
cpu     = x86

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
        core.obj

#       pgmexec.obj &

extra_incs = $(trap_dir)\lcl\dos\h
extra_cflags = /dWOW

!include $(trap_dir)\lcl\lcl.mif
!include $(trap_dir)\trap.mif

globals.obj :: ..\h\globals.h

