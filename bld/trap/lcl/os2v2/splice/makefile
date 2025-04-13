#pmake: build os_os2 lcl 32bit support x86
#
# Builds wdsplice.dll
#

tree_depth = 5

host_os  = os2
host_cpu = 386

proj_name = wdsplice

!ifndef wdsplice_autodepends
wdsplice_autodepends = .AUTODEPEND
!endif

sys_dll  = 1

!include cproj.mif
!include defrule.mif
!include dllrule.mif
!include deftarg.mif

dig_arch = $(host_cpu)

!include $(dig_dir)/client.mif

#
# cflags
#
.c: ../c

inc_dirs = -I"../h" -I"$(trap_dir)/h" -I"$(dig_dir)/h"

extra_cpp_flags = $(dig_arch_cpp_flags) -DBLDVER=$(bld_ver)

#
# lflags
#
extra_l_flags_dll = op oneautodata

# explicit rules
#################

#
# DLL
#
dlltarg_prebuild_objs = $(dll_subdir)
dlltarg_objs = $(dll_subdir)/splice.obj
!include dlltarg.mif
