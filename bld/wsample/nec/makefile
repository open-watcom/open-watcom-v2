.erase

objects = sample.obj sampdata.obj realmode.obj rmhooks.obj sampdos.obj &
          io.obj sysio.obj timasm.obj interc.obj sampexe.obj settime.obj &
          wmsg.obj sysinit.obj

cflags = /zq/ms/d1/oaxt/w3/dREPORT_TYPE=__interrupt /dFAR_PTR=__far /fiwatcom.h /i$(dig_dir)\h
cflags2 = /zu/d_NEC_PC $(cflags)

xname       = wsampnec

..\bin\$(xname).exe : $(objects) makefile $(xname).res
    @%write tmp.lnk $(%debug) option map, quiet system dos
    @%append tmp.lnk lib $(watcom_dir)\lib\wress
    @%append tmp.lnk name ..\bin\$(xname)
    @for %i in ($objects) do @%append tmp.lnk file %i

!include ..\setsize.mif
    wlink @tmp.lnk
    wstrip /q /a /r ..\bin\$(xname).exe . $(xname).res

!include ..\objsize.mif

.c: ..\dos;..\c
.c.obj :
    $(comp286) $[* $cflags2

.asm: ..\dos
.asm.obj :
    $(asm286) $[*/ZD;

$(xname).res : ..\h\wsample.rc ..\h\wmsg.h
        wrc /zk0 /r /dDOS /dNEC /fo=$^. ..\h\wsample.rc

wmsg.obj: ..\c\wmsg.c ..\h\wmsg.h
    $(comp286) $[* $cflags

clean : .symbolic
    @rm -f *.obj *.res *.lnk *.map
    @rm -f ..\bin\$(xname).exe

