.extensions:
.extensions: .exe .exp .obj .nbj .asm .h .c

objects = sample.obj samppls.obj sampdata.obj io.obj sysio.obj &
          timer.obj sampexe.obj settime.obj wmsg.obj sysinit.obj

nobjects = sample.nbj samppls.nbj sampdata.nbj io.nbj sysio.nbj &
          timer.nbj sampexe.nbj settime.nbj wmsg.nbj

cflags = /zq/mf/d1/oaxt/w3/dREPORT_TYPE= /d_PLS /dFAR_PTR= /fiwatcom.h /i$(plsdir);$(dig_dir)\h

lnk = tmp.lnk

linker = wlink

xname       = wsamppls

nxname      = nsamppls

all: at nec .symbolic
    @%null

at: ..\bin\$(xname).exp .symbolic
    @%null

nec: ..\bin\$(nxname).exp .symbolic
    @%null

..\bin\$(xname).exp : $(objects) makefile $(xname).res
    @%write $(lnk) format phar ext
    @%append $(lnk) op map
    @%append $(lnk) name ..\bin\wsamppls
    @%append $(lnk) libpath $(lang_root)\lib386\dos
    @%append $(lnk) libpath $(lang_root)\lib386
    @%append $(lnk) libpath $(plsdir)
    @%append $(lnk) libpath $(watcom_dir)\lib
    @%append $(lnk) lib clib3r,wdos32,wdosx32,wresf
    @%append $(lnk) option maxdata=0
    @%append $(lnk) runtime maxreal=1023K
    @%append $(lnk) runtime maxibuf=4K
    @%append $(lnk) runtime flag=2
    @%append $(lnk) file $(plsdir)\dilseg
    @%append $(lnk) file $(plsdir)\dilglue
    @for %i in ($(objects)) do @%append $(lnk) file %i
    $(linker) $(%debug) @$(lnk)
    wstrip /q /a /r ..\bin\$(xname).exp . $(xname).res

..\bin\$(nxname).exp : $(nobjects) makefile $(nxname).res
    @%write $(lnk) format phar ext
    @%append $(lnk) op map
    @%append $(lnk) name ..\bin\nsamppls
    @%append $(lnk) libpath $(lang_root)\lib386\dos
    @%append $(lnk) libpath $(lang_root)\lib386
    @%append $(lnk) libpath $(plsdir)
    @%append $(lnk) libpath $(watcom_dir)\lib
    @%append $(lnk) lib clib3r,wdos32,wdosx32,wresf
    @%append $(lnk) option maxdata=0
    @%append $(lnk) runtime maxreal=1023K
    @%append $(lnk) runtime maxibuf=4K
    @%append $(lnk) runtime flag=2
    @%append $(lnk) file $(plsdir)\dilseg
    @%append $(lnk) file $(plsdir)\dilglue
    @for %i in ($(objects)) do @%append $(lnk) file %i
    $(linker) $(%debug) @$(lnk)
    wstrip /q /a /r ..\bin\$(nxname).exp . $(nxname).res

.c: .;..\c

.c.obj :
    $(comp386) $[* $(cflags)

.c.nbj :
    $(comp386) $[* $(cflags)/d_NEC_PC/fo=.nbj

.asm: .;..\a

.asm.obj :
    masm $[* -i$(misc_h);

.asm.nbj :
    masm $[* -i$(misc_h) $[*.nbj;

$(xname).res : ..\h\wsample.rc ..\h\wmsg.h
        wrc /zk0 /r /dPLS /fo=$^. ..\h\wsample.rc

$(nxname).res : ..\h\wsample.rc ..\h\wmsg.h
        wrc /zk0 /r /dPLS /dNEC /fo=$^. ..\h\wsample.rc

clean : .symbolic
    @rm -f *.obj *.nbj *.res *.lnk *.map
    @rm -f ..\bin\$(xname).exp
    @rm -f ..\bin\$(nxname).exp

