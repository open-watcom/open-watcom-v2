#pmake: lcl core os_os2 cpu_386 trap x86
#
# Create the generic ELF core trap file
#

tree_depth = 5

host_cpu = 386
host_os  = os2

target_cpu = x86

srv     = lcl
name    = elf
which   = TRAP
core_entries = 1

extra_cflags = -bd

extra_objs = &
    $(elfcore_objs) &
    os22path.obj &
    supp.obj &
    core.obj

!include ../elfcore.mif
!include $(trap_dir)/lcl/os2v2/os2v2.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
