#pmake: trap lcl dos dsx 32 x86

os = nt
cpu = x86
srv = jvm

extra_objs = $(jvmx_trap_objs) jvminfo.obj acclcl.obj
extra_cflags = -dJVM -dJVMXTRAP -fh

!include $(trap_dir)\lcl\jvm\jvm.mif
!include $(trap_dir)\trap.mif
