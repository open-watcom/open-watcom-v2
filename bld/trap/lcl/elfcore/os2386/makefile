#pmake: lcl core os_os2 cpu_386 trap x86
#
# Create the generic ELF core trap file
#
host_cpu = 386
host_os  = os2

target_cpu = x86
target_os  = os2v2

extra_cflags = -bd

extra_objs = &
    $(elfcore_objs) &
    supp.obj &
    core.obj

!include ../core.mif
!include $(trap_dir)/lcl/os2v2/os2v2.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
