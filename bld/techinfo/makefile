.CONTINUE
.ERASE

!ifndef asm
asm=masm
!endif

.EXTENSIONS
.EXTENSIONS : .exe .2bj .obj .c .asm

.c.obj:
    $(comp286) $[* -bt=dos -os -s -d1 -w3

.c.2bj
    $(comp286) $[* -bt=os2 -fo=.2bj -os -s -d1 -w3 -d__OS2__ -i$(watcom_dir)\os21x_h

.asm.obj:
    $(asm) /mx /s $[@,$^@;

os2objs = techinfo.2bj ptype.obj pbus.obj pprefix.obj ndptype.obj
dosobjs = techinfo.obj ptype.obj pbus.obj pprefix.obj ndptype.obj monint.obj

techinfo.exe : makefile. dosinfo.exe $(os2objs)
    @%create $*.lnk
    @%append $*.lnk system os2 pmc
    @%append $*.lnk name $^@
    @%append $*.lnk op newfile,stub=dosinfo.exe
    @for %i in ($(os2objs)) do @%append $*.lnk file %i
    @%append $*.lnk libpath $(watcom_dir)\lib
    wlink @$*.lnk

dosinfo.exe : $(dosobjs)
    @%create $*.lnk
    @%append $*.lnk system dos
    @%append $*.lnk name $^@
    @for %i in ($(dosobjs)) do @%append $*.lnk file %i
    wlink @$*.lnk

debinfo.exe : $(dosobjs)
    @%create $*.lnk
    @%append $*.lnk system dos
    @%append $*.lnk name $^@
    @%append $*.lnk debug all op map
    @for %i in ($(dosobjs)) do @%append $*.lnk file %i
    wlink @$*.lnk
