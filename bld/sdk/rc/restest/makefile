name = restest
nament = resnt

lnk = $(name).lnk
lnknt = $(nament).lnk
cflags= -zq-s-zW-fpc-w4
cflagswin= -2-zW -fo=obj\$[&.obj -ml
cflagsnt= -mf-3-bt=nt-fo=nbj\.nbj -i=$(watcom_nt_h)
!ifndef on_build_machine
cflags += -d2
!endif

rc = wrc

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj .nbj
.EXTENSIONS: .asm .c .h .dlg

objs = restest.obj resproc.obj verinfo.obj string.obj menu.obj dialog.obj &
        data.obj cursor.obj icon.obj accel.obj bitmap.obj


ntobjs = restest.nbj resproc.nbj string.nbj menu.nbj dialog.nbj &
         data.nbj cursor.nbj icon.nbj accel.nbj bitmap.nbj msgtbl.nbj &
         verinfo.nbj


resources =  verinfo.dlg test.rc resname.dlg

$(name).exe : $(objs) $(lnk) $(name).res
        wlink @$(lnk)
        copy $(name).exe $(name)u.exe
        $(RC) -k $(name).res

$(name).res : $(name).rc $(resources)
    $(RC) -r $(name).rc -i=..\misc

$(lnk) : makefile
        %create $(lnk)
        @%append $(lnk) debug all
        @%append $(lnk) system windows
        @%append $(lnk) name $(name).exe
        @%append $(lnk) library windows.lib
        @%append $(lnk) library toolhelp.lib
        @%append $(lnk) library ver
        @%append $(lnk) option map
        @%append $(lnk) option stack=7k
        @%append $(lnk) option heapsize=2k
        @for %i in ($(objs)) do @%append $(lnk) file obj\%i

$(nament).exe : $(msgh) $(ntobjs) $(lnknt) $(nament).res
        wlink @$(lnknt)
        copy $(nament).exe $(nament)u.exe
        $(RC) -k $(nament).res

$(nament).res : $(name).rc $(resources)
    $(RC) -r $(name).rc  -d__NT__ -fo=$(nament).res -i=$(%NT_INCLUDE) -bt=nt

$(lnknt) : makefile
        %create $(lnknt)
        @%append $(lnknt) debug all
        @%append $(lnknt) sys nt_win
        @%append $(lnknt) name $(nament).exe
        @%append $(lnknt) option map
        @%append $(lnknt) option stack=64k
        @for %i in ($(ntobjs)) do @%append $(lnknt) file nbj\%i

.obj : obj
.nbj : nbj

.c : .;..\misc
.asm : .;..\misc
.dlg : .;..\misc

.c.obj :
        $(comp286) $[* $(cflags) $(cflagswin)
.c.nbj :
        $(comp386) $[* $(cflags) $(cflagsnt)
.asm.obj:
        masm /Mx /Zd $[*,obj\$[&.obj;
