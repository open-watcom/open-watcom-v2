.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .asm .c .h

objects = winmain.obj sample.obj sampdata.obj sampwin.obj io.obj &
          wmsg.obj sysio.obj sampexe.obj winopts.obj &
          fault.obj notify.obj libload.obj &
          globals.obj getidata.obj &
          segmem2.obj segmem.obj mythelp.obj inth.obj intdata.obj &
          dbg386.obj ismod32.obj

LNK = tmp.lnk

.before
        @set wcc=-i=$(sdkdir)\misc;$(dig_dir)\h -dFAR_PTR=__far -dREPORT_TYPE= /fiwatcom.h

cflags = -zu-zw-2-zq-mc-d1-orlt-s-w3

xname=wsamplew

..\bin\$(xname).exe : $(objects) $(xname).res makefile
    @%write $(lnk) option map,quiet
!ifndef on_build_machine
    @%append $(lnk) debug all op symfile
!endif
    @for %i in ($objects) do @%append $(lnk) file obj\%i
    @%append $(lnk) sys windows
    @%append $(lnk) name ..\bin\$(xname)
    @%append $(lnk) option heap=10k, stack=10k
    @%append $(lnk) option description 'WATCOM Sampler'
    @%append $(lnk) segment class 'CODE' preload fixed
    @%append $(lnk) segment class 'DATA' preload fixed
    @%append $(lnk) lib $(watcom_dir)\lib\commdlg,$(watcom_dir)\lib\toolhelp
!include ..\setsize.mif
    wlink @$(lnk)
    wrc $(xname).res ..\bin\$(xname).exe

!include ..\objsize.mif

$(xname).res : $(xname).ico $(xname).rc wsampres.rc ..\h\wsample.rc ..\h\wmsg.h
    wrc -bt=windows -zk0 -r -dWIN -30 -i$(lang_root)\h\win $(xname).rc

.c: ..\c;$(sdkdir)\misc
.asm: $(sdkdir)\misc

.obj : obj

.c.obj :
    $(comp286) $[* $(cflags) -fo=obj\.obj

.asm.obj :
        masm $[*,obj\$[&.obj;

clean : .symbolic
    @rm -f *.obj *.res *.lnk *.map *.err
    @rm -f ..\bin\$(xname).exe

