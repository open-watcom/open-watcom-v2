#pmake: lcl core os_os2 cpu_386 trap x86
#
# Create the generic ELF core trap file
#
host_cpu = 386
host_os  = os2

os      = os2v2
srv     = lcl
cpu     = x86
name    = elf 
which   = TRAP

extra_cflags = -DELFCORE -bd

extra_objs = &
        elfcore.obj &
	core_fbsd.obj &
	core_nto.obj &
	coremisc.obj &
        supp.obj &
        core.obj
	
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
