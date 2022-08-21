#pmake: lcl core os_nt cpu_386 trap x86
#
# Create the generic ELF core trap file
#

tree_depth = 5

host_cpu = 386
host_os  = nt

target_cpu = x86

srv     = lcl
name    = elf
which   = TRAP
core_entries = 1

extra_objs = &
    $(elfcore_objs) &
    ntpath.obj &
    supp.obj &
    core.obj

#extra_cppflags += -DWOW

!include ../elfcore.mif
!include $(trap_dir)/lcl/nt/nt.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
