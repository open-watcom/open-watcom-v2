.NOCHECK

.EXTENSIONS
.EXTENSIONS : .exe .obj .nbj .c .asm

.BEFORE
        set include=$(rsilib);$(%include)

model=l

xname       = wsamprsi

nxname      = nsamprsi

lf = $(%ramdisk)\tmp.lnk

cflags = /zq/m$(model)/zdp/d1/osail/s/w3/dREPORT_TYPE= /dFAR_PTR=__far /fiwatcom.h /i$(plsdir);$(dig_dir)\h

first = dos16m

objs  = $(lang_root)\lib286\dos\$(first).obj segs_16m.obj dbglib.obj &
        dbgliba.obj rmhdlr.obj sbrk.obj sample.obj samprsi.obj sampdata.obj &
        io.obj sysio.obj sampexe.obj settime.obj magicstr.obj wmsg.obj sysinit.obj

nbjs  = $(lang_root)\lib286\dos\$(first).nbj segs_16m.nbj intrm.nbj &
        dbgliba.nbj rmhdlr.nbj sbrk.nbj sample.nbj samprsi.nbj sampdata.nbj &
        io.nbj sysio.nbj sampexe.nbj settime.nbj magicstr.nbj wmsg.nbj sysinit.nbj


all: at nec .symbolic
    @%null

at: ..\bin\$(xname).exe .symbolic
    @%null

nec: ..\bin\$(nxname).exe .symbolic
    @%null

..\bin\$(xname).exe : $(xname).exp $(xname).res
    $(rsidir)splice ..\bin\$(xname).exe $(xname).exp -ALT $(rsidir)wstubq.exe
    wstrip /q /a /r ..\bin\$(xname).exe . $(xname).res

$(xname).exp : $(objs) $(__MAKEFILES__)
    @%create $(lf)
    @%append $(lf) exp(wsamprsi) map(wsamprsi) log(wsamprsi.log)
    @%append $(lf) -nod -DPMI -map -acg -farcall -quiet -auto -rtnerr 2
    @for %i in ($(objs)) do @%append $(lf) %i
    @%append $(lf) $(rsilib)\d16libc.lib $(lang_root)\lib286\dos\clib$(model).lib
    @%append $(lf) $(watcom_dir)\lib\wres$(model)p.lib
    $(rsidir)glu @$(lf)
    if exist *.err del *.err

..\bin\$(nxname).exe : $(nxname).exp $(nxname).res
    $(rsidir)splice ..\bin\$(nxname).exe $(nxname).exp -ALT $(rsidir)wstubq.exe
    wstrip /q /a /r ..\bin\$(nxname).exe . $(nxname).res

nsamprsi.exp : $(nobjs) $(__MAKEFILES__)
    @%create $(lf)
    @%append $(lf) exp(nsamprsi) map(nsamprsi) log(nsamprsi.log)
    @%append $(lf) -nod -DPMI -map -acg -farcall -auto -rtnerr 2
    @for %i in ($(objs)) do @%append $(lf) %i
    @%append $(lf) $(rsilib)\d16libc.lib $(lang_root)\lib286\dos\clib$(model).lib
    @%append $(lf) $(watcom_dir)\lib\wres$(model)p.lib
    $(rsidir)glu @$(lf)
    if exist *.err del *.err

.c: .;..\c;$(rsilib)

.c.obj :
    $(comp286) $[* $(cflags)

.c.nbj :
    $(comp286) $[* $(cflags)/d_NEC_PC/fo=.nbj

.asm: .;..\a;$(rsilib)

.asm.obj :
    masm /mx /s /Dlarge $[@;

.asm.nbj :
    masm /mx /s /Dlarge $[@.nbj;

$(xname).res : ..\h\wsample.rc ..\h\wmsg.h
        wrc /zk0 /r /dRSI  /fo=$^. ..\h\wsample.rc
$(nxname).res : ..\h\wsample.rc ..\h\wmsg.h
        wrc /zk0 /r /dRSI /dNEC /fo=$^. ..\h\wsample.rc

clean : .symbolic
    @rm -f *.obj *.res *.lnk *.map *.exp *.log *.err
    @rm -f ..\bin\$(xname).exe
    @rm -f ..\bin\$(nxname).exe

