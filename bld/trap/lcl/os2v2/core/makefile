#pmake: lcl os_os2 cpu_386 trap x86

tree_depth = 5

host_os  = os2
host_cpu = 386

target_cpu = x86

srv          = lcl
name         = core
which        = TRAP
core_entries = 1

#
# Create the 32-bit OS/2 core.dll
#

extra_cflags = -bd

extra_objs = &
    elfcore.obj &
    coremisc.obj &
    os22path.obj &
#    accmisc.obj &
    x86cpu.obj &
    supp.obj &
    core.obj

!include $(trap_dir)/lcl/elfcore/elfcore.mif
!include $(trap_dir)/lcl/os2v2/os2v2.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
