objects = sample.obj sampdata.obj realmode.obj sampecs.obj io.obj sysio.obj &
          rmhooks.obj interc.obj memory.obj rmkint.obj sampexe.obj settime.obj &
          wmsg.obj

cflags = /zq/ms/d1/oaxt/w3/dFAR=__far /dFAR_PTR=__far /dREPORT_TYPE=__interrupt /fiwatcom.h /i$(ecsdir);$(dig_dir)\h
cflags2 = /zu $(cflags)

assemble = $(asm286) /Doscode=_TEXT /DTinyModel /DUniProcessor /I$(ecsdir)\ $[@;

xname=wsampecs
jname=wsmpecsj

..\bin\$(xname).exe ..\bin\$(jname).exe : $(objects) makefile $(xname).res $(jname).res
    @%write tmp.lnk $(%debug) option map,quiet form dos name ..\bin\$(xname)
    @%append tmp.lnk lib $(lang_root)\lib286\dos\clibs
    @%append tmp.lnk lib $(watcom_dir)\lib\wress
    @for %i in ($(objects)) do @%append tmp.lnk file %i
!include ..\setsize.mif
    wlink @tmp.lnk
    copy ..\bin\$(xname).exe ..\bin\$(jname).exe
    wstrip /q /a /r ..\bin\$(xname).exe . $(xname).res
    wstrip /q /a /r ..\bin\$(jname).exe . $(jname).res

!include ..\objsize.mif

.c: .;..\dos;..\c
.c.obj :
    $(comp286) $[* $cflags2

.asm: ..\dos;$(ecsdir)
.asm.obj:
        $(assemble)

$(xname).res : ..\h\wsample.rc ..\h\wmsg.h
        wrc /r /dECS /dENGLISH /fo=$^. ..\h\wsample.rc

$(jname).res : ..\h\wsample.rc ..\h\wmsg.h
        $(wrck) /r /dECS /dJAPANESE /fo=$^. ..\h\wsample.rc

wmsg.obj: ..\c\wmsg.c ..\h\wmsg.h
    $(comp286) $[* $cflags
