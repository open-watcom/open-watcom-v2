#pmake: build os_os2 lcl 32bit support x86
#
# Builds wdsplice.dll
#

host_os  = os2
host_cpu = 386
sys_dll  = 1

!include cproj.mif
!include defrule.mif
!include deftarg.mif

objs = splice.obj

extra_l_flags = op oneautodata, map

inc_dirs = -I"$(dig_dir)/h"

.c: ../c

wdsplice.dll : $(objs)
    $(linker) $(lflags) name $@ file { $< } l $(os2386)
