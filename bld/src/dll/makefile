# wmake lang=c
# wmake lang=cpp

!ifndef lang
lang = c
!endif

!ifeq lang c
compile286 = wcc
compile386 = wcc386
!endif

!ifeq lang cpp
compile286 = wpp
compile386 = wpp386
!endif

dllflags = -bd -zc
cflags = -bt=windows -d1

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
.EXTENSIONS: .dll .exe .obj .c

.BEFORE
    set include=$(%watcom)\h\win;$(%include)

all : dll16.dll dll32.dll gen16.exe gen32.exe .SYMBOLIC

gen16.exe : gen16.obj
    wlink @gen16

dll16.dll : dll16.obj
    wlink @dll16

gen32.exe : gen32.obj
    wlink @gen32
    wbind -n gen32
    @del gen32.rex

dll32.dll : dll32.obj
    wlink @dll32
    wbind -d -n dll32
    @del dll32.rex

dll16.obj : dll16.c
    $(compile286) dll16 -mc -zu $(dllflags) $(cflags)

dll32.obj : dll32.c
    $(compile386) dll32 $(dllflags) $(cflags)

gen16.obj : gen16.c
    $(compile286) gen16 $(cflags)

gen32.obj : gen32.c
    $(compile386) gen32 $(cflags)

# rules for making VBDLL32.DLL and COVER16.DLL

vbdll32.dll : vbdll32.rex cover16.dll
    wbind vbdll32 -q -d -n -s $(%watcom)\binw\w386dll.ext

vbdll32.rex : vbdll32.obj
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts_32)
    wlink @tmp.lnk file vbdll32
    @del tmp.lnk

vbdll32.obj : vbdll32.c
    $(compile386) vbdll32 $(dllflags) $(cflags)

cover16.dll : cover16.obj
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts_16)
    @%append tmp.lnk system windows_dll
    wlink @tmp.lnk file cover16 option description '16-bit DLL Cover Functions'
    @del tmp.lnk

cover16.obj : cover16.c
    $(compile286) cover16 -mc -zu $(dllflags) $(cflags)

clean : .SYMBOLIC
        @if exist *.exe del *.exe
        @if exist *.dll del *.dll
        @if exist *.obj del *.obj
        @if exist *.map del *.map
