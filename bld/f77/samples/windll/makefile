compile286 = wfc
f16flags = -quiet -explicit -nowarn -d2 -windows

compile386 = wfc386
f32flags = -quiet -explicit -nowarn -d2

c286 = wcc
c16flags = -zq -mc -bd -zu -d2 -bt=windows

dllflags = -bd

link_opts_16 = &
    debug all &
    option map &
    option quiet &
    option stack=8K &
    option heapsize=32K

link_opts_32 = &
    debug all &
    system win386 &
    option map &
    option quiet &
    option stack=12K &
    option maxdata=8K &
    option mindata=4K

.EXTENSIONS:
.EXTENSIONS: .dll .exe .rex .obj .for .c

.BEFORE
    set finclude=$(%watcom)/src/fortran/win;$(%finclude)

all : gen16.exe gen32.exe gen16v.exe gen32v.exe .SYMBOLIC
    @echo All done! Install and run the examples under Windows.

gen16.exe : gen16.obj windll.dll
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts_16)
    @%append tmp.lnk system windows
    wlink @tmp.lnk file gen16 option description '16-bit DLL Test'
    @del tmp.lnk

gen16v.exe : gen16v.obj windllv.dll
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts_16)
    @%append tmp.lnk system windows
    wlink @tmp.lnk file gen16v option description '16-bit DLL Test (v)'
    @del tmp.lnk

gen32.exe : gen32.rex windll.dll
    wbind gen32 -q -n -D "32-bit DLL Test" -s $(%watcom)\binw\win386.ext

gen32.rex : gen32.obj
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts_32)
    wlink @tmp.lnk file gen32
    @del tmp.lnk

gen32v.exe : gen32v.rex windllv.dll
    wbind gen32v -q -n -D "32-bit DLL Test (v)" -s $(%watcom)\binw\win386.ext

gen32v.rex : gen32v.obj
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts_32)
    wlink @tmp.lnk file gen32v
    @del tmp.lnk

windll.dll : windll.rex
    wbind windll -q -d -n -s $(%watcom)\binw\w386dll.ext

windll.rex : windll.obj
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts_32)
    wlink @tmp.lnk file windll
    @del tmp.lnk

windllv.dll : windllv.rex
    wbind windllv -q -d -n -s $(%watcom)\binw\w386dll.ext

windllv.rex : windllv.obj
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts_32)
    wlink @tmp.lnk file windllv
    @del tmp.lnk

gen16.obj : gen16.for
    $(compile286) gen16 $(f16flags)

gen16v.obj : gen16v.for
    $(compile286) gen16v $(f16flags)

gen32.obj : gen32.for
    $(compile386) gen32 $(f32flags)

gen32v.obj : gen32v.for
    $(compile386) gen32v $(f32flags)

windll.obj : windll.for
    $(compile386) windll $(dllflags) $(f32flags)

windllv.obj : windllv.for
    $(compile386) windllv $(dllflags) $(f32flags)

# rules for making VBDLL32.DLL and COVER16.DLL

vbdll32.dll : vbdll32.rex cover16.dll
    wbind vbdll32 -q -d -n -s $(%watcom)\binw\w386dll.ext

vbdll32.rex : vbdll32.obj
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts_32)
    wlink @tmp.lnk file vbdll32
    @del tmp.lnk

vbdll32.obj : vbdll32.for
    $(compile386) vbdll32 $(dllflags) $(f32flags)

cover16.dll : cover16.obj
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts_16)
    @%append tmp.lnk system windows_dll
    wlink @tmp.lnk file cover16 option description '16-bit DLL Cover Functions'
    @del tmp.lnk

cover16.obj : cover16.c
    set include=$(%watcom)/h;$(%watcom)/h/win;$(%include)
    $(c286) cover16 $(dllflags) $(c16flags)


clean : .SYMBOLIC
        @if exist *.exe del *.exe
        @if exist *.rex del *.rex
        @if exist *.dll del *.dll
        @if exist *.obj del *.obj
        @if exist *.map del *.map

