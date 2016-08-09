host_os  = dos
host_cpu = i86

proj_name = testovl

testovl_debugfmt = watcom

!include cproj.mif
!include defrule.mif
!include deftarg.mif

extra_c_flags = -s $(%COPTIONS)

OBJS = main.obj ovl1.obj ovl2.obj ovl3.obj

all : dmain.exe lmain.exe smain.exe .symbolic

dmain.exe : .always
    rm -f *.obj
    set COPTIONS=-ml -of -DDYNOVLMGR
    %make main.obj
    %make ovl1.obj
    %make ovl2.obj
    %make ovl3.obj
    $(linker) n $@ $(lflags) op dynamic begin autosection file { $(OBJS) } end

lmain.exe : .always
    rm -f *.obj
    set COPTIONS=-ml
    %make main.obj
    %make ovl1.obj
    %make ovl2.obj
    %make ovl3.obj
    $(linker) n $@ $(lflags) sort global op standard begin autosection file { $(OBJS) } end

smain.exe : .always
    rm -f *.obj
    set COPTIONS=-ms
    %make main.obj
    %make ovl1.obj
    %make ovl2.obj
    %make ovl3.obj
    $(linker) n $@ $(lflags) sort global op small begin autosection file { $(OBJS) } end
