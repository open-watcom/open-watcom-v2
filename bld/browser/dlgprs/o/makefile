#pmake/20: build os_nt os_os2 os_osi os_win cpu_axp cpu_i86 cpu_386

host_cpu = $(bld_cpu)
host_os  = $(bld_os)

proj_name = dlgprs

!ifndef dlgprs_autodepends
dlgprs_autodepends=.AUTODEPEND
!endif

.IGNORE

.EXTENSIONS: .gh

!include cproj.mif
!include deftarg.mif
!include defrule.mif

.cpp: .;../cpp;$(browse_dir)/cpp

scan_files = &
bind.obj &
dialog.obj &
main.obj &
scanner.obj &
styles.obj &
prsbnd.obj &
prsdlg.obj

browse_files = &
chfile.obj  &
chbffile.obj &
mempool.obj

c_objs = $+$(scan_files) $(browse_files)$-

lnk = dlgprs.lnk

extra_c_flags=-xs

extra_l_flags=op map

#Note that this project need to include windows.h for styles.cpp on every platform
extra_c_flags_styles_dos   = $(inc_dirs_sys_win)
extra_c_flags_styles_os2   = $(inc_dirs_sys_win)
extra_c_flags_styles_linux = $(inc_dirs_sys_win)
!ifdef extra_c_flags_styles_$(host_os)
extra_c_flags_styles = $(extra_c_flags_styles_$(host_os))
!endif

inc_dirs = -I. -I"../h" -I"$(browse_dir)/h"

dlgprs.exe : prsdlg.gh prsbnd.gh $(c_objs)
!ifndef __WATCOM__
    $(cl) $(clflags) $(c_objs) $(ldflags)
!else
    $(linker) $(lflags) name $^@ file {$(c_objs)}
!endif

prsdlg.gh prsdlg.gc : ../y/prsdlg.y ../cpp/yydriver.cpp
    $(yacc) -b $[@ $]@ >$(nulldevice)
    @rm -f prsdlg.gh
    @cp ytab.h prsdlg.gh >$(nulldevice)
    @rm -f ytab.h
    @sed -e "s/^static //" ytab.c >prsdlg.cpp
    @rm -f ytab.c

prsbnd.gh prsbnd.gc : ../y/prsbnd.y ../cpp/yydriver.cpp
    $(yacc) -b $[@ $]@ >$(nulldevice)
    @rm -f prsbnd.gh
    @cp ytab.h prsbnd.gh >$(nulldevice)
    @rm -f ytab.h
    @sed -e "s/^static //" ytab.c >prsbnd.cpp
    @rm -f ytab.c

