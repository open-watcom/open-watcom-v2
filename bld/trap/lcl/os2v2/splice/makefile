#pmake: build os_os2 lcl 32bit support x86
#
# Builds wdsplice.dll
#

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

#
# cflags
#
.c: ../c

inc_dirs = -I"../h" -I"$(trap_dir)/h" -I"$(dig_dir)/h"

extra_cpp_flags = -DMD_x86

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
