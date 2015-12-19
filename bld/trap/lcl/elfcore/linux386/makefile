#pmake: lcl core os_linux cpu_386 trap x86
#
# Create the generic ELF core trap file
#
host_cpu = 386
host_os  = linux

target_cpu = x86

os      = linux
srv     = lcl
name    = elf
which   = TRAP

extra_cpp_flags = -DELFCORE

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
