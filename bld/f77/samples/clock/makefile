# wmake type=16
# wmake type=32

!ifndef type
type = 32
!endif

!ifeq type 16
compile = wfc
flags = -quiet -nowarn -d2 -windows
link_opts = &
    debug all &
    system windows memory font &
    option map &
    option quiet &
    option stack=8K &
    option heapsize=32K &
    option description 'WATCOM F77/16 Clock'
!endif

!ifeq type 32
compile = wfc386
flags = -quiet -nowarn -d2
link_opts = &
    debug all &
    system win386 &
    option map &
    option quiet &
    option stack=12K &
    option maxdata=8K &
    option mindata=4K
!endif

.EXTENSIONS:
.EXTENSIONS: .exe .obj .for

.BEFORE
    set finclude=$(%watcom)\src\fortran\win;$(%finclude)

all : clock$(type).exe .SYMBOLIC
    @echo All done! Install and run CLOCK$(type) under Windows.

clock$(type).exe : clock$(type).obj
    @%create tmp.lnk
    @%append tmp.lnk $(link_opts)
    wlink @tmp.lnk file clock$(type)
!ifeq type 32
    wbind clock$(type) -q -n -D "WATCOM F77/32 Clock"
    @del clock$(type).rex
!endif
    @del tmp.lnk

clock$(type).obj : clock.for
    $(compile) $< $(flags) -fo=$@

clean : .SYMBOLIC
    @if exist *.exe del *.exe
    @if exist *.obj del *.obj
    @if exist *.map del *.map
