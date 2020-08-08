#pmake: nobuild guitool os_dos cpu_386

proj_name = defgen

host_os  = dos
host_cpu = 386

additional_clean = ytab.*

!include cproj.mif
!include defrule.mif
!include deftarg.mif

#
# cflags
#
.c: ..

inc_dirs = -I. -I..

# explicit rules
#################

#
# EXE
#
exetarg_prebuild_objs = ytab.h ytab.c
exetarg_objs          = ytab.obj main.obj scan.obj error.obj semantic.obj output.obj
!include exetarg.mif

#
# rc and others
#
ytab.c ytab.h: ../yydriver.c ../defgen.y
    @%make echo_yacc
    $(yacc) -d -dd $]@ $[@
