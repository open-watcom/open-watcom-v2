host_os  = dos
host_cpu = i86

proj_name = testovl

testovl_debugfmt = watcom

!include cproj.mif
!include defrule.mif
!include deftarg.mif

extra_c_flags = -s $(%COPTIONS)

OBJS = main.obj ovl1.obj ovl2.obj ovl3.obj

all : .symbolic
    -wmake -h dmain.exe release=$(release)
    -wmake -h lmain.exe release=$(release)
    -wmake -h smain.exe release=$(release)

dmain.exe : .always
    rm -f *.obj
    set COPTIONS=-ml -of -DDYNOVLMGR
    %make main.obj
    %make ovl1.obj
    %make ovl2.obj
    %make ovl3.obj
    $(linker) n $@ $(lflags) op map, dynamic begin autosection file { $(OBJS) } end libpath ../ovlldr/dosi86

lmain.exe : .always
    rm -f *.obj
    set COPTIONS=-ml
    %make main.obj
    %make ovl1.obj
    %make ovl2.obj
    %make ovl3.obj
    $(linker) n $@ $(lflags) sort global op map, standard begin autosection file { $(OBJS) } end libpath ../ovlldr/dosi86

smain.exe : .always
    rm -f *.obj
    set COPTIONS=-ms
    %make main.obj
    %make ovl1.obj
    %make ovl2.obj
    %make ovl3.obj
    $(linker) n $@ $(lflags) sort global op map, small begin autosection file { $(OBJS) } end libpath ../ovlldr/dosi86
