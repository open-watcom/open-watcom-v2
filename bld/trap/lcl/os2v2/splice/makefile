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
!include deftarg.mif

dig_arch = $(host_cpu)

!include $(dig_dir)/digcli.mif

#
# cflags
#
.c: ../c

inc_dirs = -I"../h" -I"$(trap_dir)/h" -I"$(dig_dir)/h"

extra_cpp_flags = $(dig_arch_cpp_flags)

#
# lflags
#
extra_l_flags_dll = op oneautodata

# explicit rules
#################

#
# DLL
#
dlltarg_objs = splice.obj
!include dlltarg.mif
