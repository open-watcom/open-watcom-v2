name = defgen
lnk = $(name).lnk


objs = ytab.obj main.obj scan.obj error.obj semantic.obj output.obj

cflags = -w4 -mf -d2 -fo=obj\$[&.obj

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .asm .c .h

$(name).exe : $(objs) $(lnk)
        wlink @$(lnk)
        $(pharlap)bind386 $(pharlap)run386b.exe $^*.exp -exe $^@
        @del $^*.exp

$(name).lnk : makefile
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) system pharlap
        @%append $(lnk) runtime maxreal=640k
        @%append $(lnk) op maxdata=1024K
        @%append $(lnk) name $(name).exp
        @%append $(lnk) option map
        @for %i in ($(objs)) do @%append $(lnk) file obj\%i

ytab.c : yydriver.c defgen.y
        yacc -d defgen.y yydriver.c

.obj : obj

.c.obj :
        $(comp386) $[* $(cflags)
