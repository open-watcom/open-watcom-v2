!ifdef codeview
copts = /d1 /hc $(addinc)
!else
copts = /d2 $(addinc)
!endif
#       /w4 /fpc # fpc is necessary for all AUI apps!!
.before:
        set include=$(auidir)\h;$(guidir)\h;$(uidir)\h;..\h;$(%include)

rcfiles = &
    $(guidir)\h\gui.rc &
    $(auidir)\h\aui.rc &
    $(auidir)\sample\h\app.rc &
    $(auidir)\sample\h\appcomm.rc &
    $(auidir)\sample\h\dlgoptn.h &
    $(auidir)\sample\h\dlgoptn.dlg &
    ..\one.bmp &
    ..\two.bmp &
    ..\three.bmp &
    ..\four.bmp &
    ..\five.bmp &

app.res appj.res : $(rcfiles) ..\one.bmp ..\two.bmp ..\three.bmp ..\four.bmp ..\five.bmp
        wrc $(rcopt) -r -q ..\h\app.rc /foapp.res
        wrc $(rcopt) -r -q ..\h\app.rc /foappj.res

appui.res appuij.res : $(rcfiles) ..\h\appui.rc
        wrc -bt=windows -r -q ..\h\appui.rc /foappui.res
        wrc -dJAPANESE_MESSAGES -bt=windows -r -q ..\h\appui.rc /foappuij.res

apppm.res apppmj.res : $(rcfiles) ..\h\apppm.rc
        @set oinc=$(%include)
        set include=$(auidir)\pm;$(%include);$(watcom_dir)\os21x_h
        wpp -p -fo=apppm.i ..\h\apppm.rc
        wpp -dJAPANESE_MESSAGES -p -fo=apppmj.i ..\h\apppm.rc
        set opath=$(%path)
        set path=$(lang_root)\binp
        $(lang_root)\binp\rc -r app.rc
        del apppm.i
        copy app.res apppm.res
        rename apppmj.i apppm.i
        $(lang_root)\binp\rc -r app.rc
        copy app.res apppmj.res
        set path=$(%opath)
        @set include=$(%oinc)

.c: ..\c
.c.obj:
        $(compile) $(copts) /zq /fh

eraseall.obj : $(auidir)\compile.gbl
        if exist *.obj erase *.obj
        wtouch eraseall.obj

objs = app.obj util.obj window1.obj window2.obj window3.obj &
       window4.obj window5.obj window6.obj window7.obj window8.obj &
       menus.obj appcolor.obj dlgcmd.obj dlgoptn.obj password.obj

app.lnk : eraseall.obj $(objs) $(__makefiles__) $(auilib)
!ifdef codeview
        @%write app.lnk debug codeview
!else
        @%write app.lnk debug all
!endif
        @%append app.lnk op map,un,stack=12k
        @for %i in ($(objs)) do @%append app.lnk file %i
        @%append app.lnk lib $(auilib)
