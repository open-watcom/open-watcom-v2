#pmake: build os_win cpu_386
copts = -ml -bt=windows -w4 -bd -zu
lnk = vpdll.lnk
objs = vpdll.obj

!ifndef on_build_machine
copts += -d2
!endif

vpdll.dll : vpdll.lnk $(objs)
        wlink @$(lnk)

vpdll.lnk : makefile
        %create $(lnk)
        @%append $(lnk) debug all
        @%append $(lnk) sys windows_dll
        @%append $(lnk) op map
        @%append $(lnk) libr ddeml
        @for %i in ($(objs)) do @%append $(lnk) file %i

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .c

.c : .
.c.obj
        wcc $(copts) $[*
