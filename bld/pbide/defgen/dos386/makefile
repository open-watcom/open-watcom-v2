#pmake: nobuild os_dos cpu_386

proj_name = defgen

host_os  = dos
host_cpu = 386

xname = defgen.exe

lnk = $(name).lnk

!include cproj.mif
!include defrule.mif
!include deftarg.mif

inc_dirs = -I..

objs = ytab.obj main.obj scan.obj error.obj semantic.obj output.obj

$(xname) : ytab.h ytab.c $(objs) $(__MAKEFILES__)
        $(cl) $(clflags) $(objs) $(ldflags)

.c: ..

ytab.c ytab.h: ../yydriver.c ../defgen.y
        $(yacc) -d $]@ $[@

additional_cleanup = ytab.h ytab.c
