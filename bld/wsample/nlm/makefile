objects = realmode.obj nwhooks.obj sample.obj sampdata.obj sampnlm.obj &
          io.obj sysio.obj settime.obj wmsg.obj

cflags = /bt=netware/3s/mf/d1/oax/w3 /dREPORT_TYPE= /dFAR_PTR= /dNETWARE /fiwatcom.h /zq

.BEFORE
        set include=$(nwdir);$(nwsrc);$(dig_dir)\h;$(%include)

imports = &
    FileServerMajorVersionNumber &
    AllocateResourceTag &
    ScheduleSleepAESProcessEvent &
    MapAbsoluteAddressToDataOffset &
    Abend  &
    Pause  &
    Disable  &
    Enable  &
    RegisterForEventNotification  &
    UnRegisterEventNotification  &
    systemConsoleScreen  &
    OutputToScreen  &
    LoadModule  &
    LoadedList &
    Alloc &
    Free

xname = wsample

..\bin\$(xname).nlm : $(xname).res  $objects makefile
    @%write $(%debug) tmp.lnk name ..\bin\$(xname)
    @%append tmp.lnk system novell nlm 'WATCOM Execution Sampler Version 1.0'
#    @%append tmp.lnk module CLIB
#    @%append tmp.lnk debug novell
    @%append tmp.lnk option map
    @%append tmp.lnk option caseexact
    @%append tmp.lnk option quiet
#    @%append tmp.lnk option case
    @%append tmp.lnk option stack=10k
#    @%append tmp.lnk file $(nwdir)\prelude
#    @%append tmp.lnk import @$(nwdir)\clib.imp
#    @%append tmp.lnk import @$(nwdir)\mathlib.imp
    @%append tmp.lnk lib $(watcom_dir)\lib\wresfs
    @for %i in ($(imports)) do @%append tmp.lnk import %i
    @for %i in ($objects) do @%append tmp.lnk file %i
!include ..\setsize.mif
    wlink @tmp.lnk
    wstrip /q /a /r ..\bin\$(xname).nlm . $(xname).res

!include ..\objsize.mif

.c: .;..\c

.c.obj :
    $(comp386) $[* $cflags

.asm.obj:
    $(asm386) $^& -nolist -twocase -errorlist $^&.err -include \dev\h\
    erase $^&.err

$(xname).res : ..\h\wsample.rc ..\h\wmsg.h
        wrc /zk0 /r /dNLM /fo=$^. ..\h\wsample.rc

clean : .symbolic
    @rm -f *.obj *.res *.lnk *.map *.err
    @rm -f ..\bin\$(xname).nlm
