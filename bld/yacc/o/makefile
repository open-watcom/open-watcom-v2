proj_name = yacc
host_CPU = 386
!include cproj.mif
.erase

common_objs = &
    bitsets.obj &
    buildpro.obj &
    default.obj &
    dont_opt.obj &
    elimunit.obj &
    fpopen.obj &
    getname.obj &
    lalr1.obj &
    lr0.obj &
    parse.obj &
    sentence.obj &
    used.obj &
    yacc.obj

table_objs = gentable.obj genfast.obj gensupp.obj
ins_objs = genobj.obj writeobj.obj
small_objs = gensmall.obj

dos_386_lnk = SYSTEM dos4g OPTION offset=64k
os2_386_lnk = SYSTEM os2v2 pmc
nt_386_lnk = SYSTEM nt OPTION offset=64k

.c: ..\c
.c.obj:
    $(cc_386) $[* -oaxlt -s -d1 -mf -w4 -zq -zp4

!ifdef __dos__
default_yacc = yacc.exe
!else ifdef __os2__
default_yacc = yacc2.exe
!else ifdef __nt__
default_yacc = yaccnt.exe
!else
default_yacc = yacc.exe
!endif

first : .symbolic $(default_yacc)
    @%null

all : .symbolic yacc.exe yacc2.exe yaccnt.exe
    @%null

syacc.exe : $(common_objs) $(small_objs) syacc.lnk
    wlink DEBUG line $(dos_386_lnk) NAME $^* @$]@

iyacc.exe : $(common_objs) $(ins_objs) iyacc.lnk
    wlink DEBUG line $(dos_386_lnk) NAME $^* @$]@

yacc2.exe : $(common_objs) $(table_objs) yacc.lnk
    wlink DEBUG line $(os2_386_lnk) NAME $^* @$]@

yaccnt.exe : $(common_objs) $(table_objs) yacc.lnk
    wlink DEBUG line $(nt_386_lnk) NAME $^* @$]@

yacc.exe : $(common_objs) $(table_objs) yacc.lnk
    wlink DEBUG line $(dos_386_lnk) NAME $^* @$]@

yacc.lnk : makefile
    @%write $^@ OPTION quiet, map
    @for %i in ( $(common_objs) $(table_objs) ) do @%append $^@ FILE %i

iyacc.lnk : makefile
    @%write $^@ OPTION quiet, map
    @for %i in ( $(common_objs) $(ins_objs) ) do @%append $^@ FILE %i

syacc.lnk : makefile
    @%write $^@ OPTION quiet, map
    @for %i in ( $(common_objs) $(small_objs) ) do @%append $^@ FILE %i

global : .symbolic
    rm -f *.obj *.ex? *.map *.lnk ytab.* *.tmp
