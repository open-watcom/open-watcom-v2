#pmake: all os_dos os_os2 cpu_i86 compact build

proj_name = wlink
host_OS = dos
host_CPU = i86

!include cproj.mif
!include deftarg.mif

template = &
fileio.$(ext) &
keyword.$(ext) &
mem.$(ext) &
ms2wlink.$(ext) &
parsems.$(ext) &
utils.$(ext)

ext = obj
os2_objs = $+$(template)$-

ext = dbj
dos_objs = $+$(template)$-
.extensions:
.extensions: .exe .obj .dbj .c

copts   = /fpc/oaxt /mc /d1 /w4 /we

!ifdef on_build_machine
copts += -zq
!endif

compile = $(cc_i86) $[* $(copts)
proj = ms2wlink
dosproj = ms2wldos

prj = ms2wlink.exe

.c: ..\c

.c.obj:
        $(compile) /bt=os2

.c.dbj:
        $(compile) /bt=dos /fo=.dbj

$(prj) : $(os2_objs) $(dosproj).exe
        $(linker) system os2 pmcompat n $(prj) op map f {$(os2_objs)} op stub=$(dosproj).exe op stack=8k

$(dosproj).exe : $(dos_objs)
        $(linker) system dos n $(dosproj) op map f {$(dos_objs)}
