#pmake: lcl core os_os2 cpu_386 trap x86
#
# Create the generic ELF core trap file
#
host_cpu = 386
host_os  = os2

target_cpu = x86
target_os  = os2v2

srv     = lcl
name    = elf
which   = TRAP

extra_cpp_flags = -DELFCORE
extra_cflags = -bd

extra_objs = &
        elfcore.obj &
        core_fbsd.obj &
        core_nto.obj &
        coremisc.obj &
        supp.obj &
        core.obj

inc_dirs   = -I"$(trap_dir)/lcl/elfcore/h"

!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
