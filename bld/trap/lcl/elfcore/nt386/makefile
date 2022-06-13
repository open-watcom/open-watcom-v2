#pmake: lcl core os_nt cpu_386 trap x86
#
# Create the generic ELF core trap file
#
host_cpu = 386
host_os  = nt

target_cpu = x86
target_os  = nt

srv     = lcl
name    = elf
which   = TRAP
core_entries = 1

extra_objs = &
    $(elfcore_objs) &
    supp.obj &
    core.obj

!include ../elfcore.mif
!include $(trap_dir)/lcl/nt/nt.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
