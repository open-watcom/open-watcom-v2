compile286 = wfc
f16flags = -quiet -explicit -nowarn -d2 -windows

compile386 = wfc386
f32flags = -quiet -explicit -nowarn -d2

dllflags = -bd

link_opts_16 = &
    debug all &
    option map &
    option quiet &
    option heapsize=32K

link_opts_32 = &
    debug all &
    system win386 &
    option map &
    option quiet &
    option maxdata=8K &
    option mindata=4K

.EXTENSIONS:
.EXTENSIONS: .dll .exe .rex .obj .for .c

.BEFORE
    set finclude=$(%watcom)/src/fortran/win;$(%finclude)

all : ellips16.exe ellips32.exe fwcopy16.exe fwcopy32.exe .SYMBOLIC
    @echo All done! Run the examples under Windows!

ellips16.exe : ellips16.obj
    *wlink $(link_opts_16) system windows file ellips16 option description '16-bit Ellipse.'

fwcopy16.exe : fwcopy16.obj
    *wlink $(link_opts_16) system windows file fwcopy16 option description '16-bit File Copy.'

fwcopy32.exe : fwcopy32.rex
    *wbind fwcopy32 -q -n -D "32-bit File Copy" -s $(%watcom)\binw\win386.ext

fwcopy32.rex : fwcopy32.obj
    *wlink $(link_opts_32) file fwcopy32

ellips32.exe : ellips32.rex
    *wbind ellips32 -q -n -D "32-bit Ellipse" -s $(%watcom)\binw\win386.ext

ellips32.rex : ellips32.obj
    *wlink $(link_opts_32) file ellips32

fwcopy32.obj : fwcopy.for
        *$(compile386) fwcopy -fo=fwcopy32 $(f32flags)

fwcopy16.obj : fwcopy.for
        *$(compile286) fwcopy -fo=fwcopy16 $(f16flags)

ellips16.obj : ellipse.for
        *$(compile286) ellipse -fo=ellips16 $(f16flags)

ellips32.obj : ellipse.for
        *$(compile386) ellipse -fo=ellips32 $(f32flags)

clean : .SYMBOLIC
        @if exist *.exe del *.exe
        @if exist *.rex del *.rex
        @if exist *.dll del *.dll
        @if exist *.obj del *.obj
        @if exist *.map del *.map

