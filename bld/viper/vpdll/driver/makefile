name = wvp
lnk = $(name).lnk

objs = new.obj

$(name).exe : $(objs) $(lnk) new.res
        wlink @$(lnk)
        wrc new.res $(name).exe

new.res : new.rc
        wrc -bt=windows -i=$(%watcom)\h\win -r new.rc -fo=new.res

$(lnk) : makefile
        %create $(lnk)
        @%append $(lnk) DEBUG ALL
        @%append $(lnk) sys windows
        @%append $(lnk) NAME $(name).exe
        @%append $(lnk) OP MAP
        @for %i in ($(objs)) do @%append $(lnk) file %i

.obj : .;
.c : .;

.c.obj :
        wcc -bt=windows -zW -ml -d2 -w4 -i=$(%watcom)\h\win;..\dll $[*
