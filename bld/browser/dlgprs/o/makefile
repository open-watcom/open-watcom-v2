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

scan_files = &
bind.obj &
dialog.obj &
main.obj &
scanner.obj &
styles.obj &
prsbnd_c.obj &
prsdlg_c.obj

browse_files = &
chfile.obj  &
chbffile.obj &
mempool.obj

c_objs = $+$(scan_files) $(browse_files)$-

lnk = dlgprs.lnk

extra_c_flags=-xs
extra_l_flags=op map

#Note that this project includes windows.h on every platform
inc_dirs = -I"$(browse_dir)/h" -I"../h" $(inc_dirs_sys_win)

dlgprs.exe : prsdlg.gh prsbnd.gh $(c_objs) $(browse_objs)
    $(linker) $(lflags) name $^@ file {$(c_objs)}

prsdlg.gh prsdlg_c.gh : ../y/prsdlg.y ../cpp/yydriver.cpp
    $(yacc) -b $[@ $]@ >$(nulldevice)
    @rm -f prsdlg.gh
    @cp ytab.h prsdlg.gh >$(nulldevice)
    @rm -f ytab.h
    @rm -f prsdlg_c.gh
    @cp ytab.c prsdlg_c.gh >$(nulldevice)
    @rm -f ytab.c

prsbnd.gh prsbnd_c.gh : ../y/prsbnd.y ../cpp/yydriver.cpp
    $(yacc) -b $[@ $]@ >$(nulldevice)
    @rm -f prsbnd.gh
    @cp ytab.h prsbnd.gh >$(nulldevice)
    @rm -f ytab.h
    @rm -f prsbnd_c.gh
    @cp ytab.c prsbnd_c.gh >$(nulldevice)
    @rm -f ytab.c

.cpp: ../cpp;$(browse_dir)/cpp

.gh.obj: $(dlgprs_autodepends)
    $(cxx) $(cflags) $(inc_path) $[@
