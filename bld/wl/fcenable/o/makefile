#pmake: all os_dos os_os2 cpu_i86 small build

proj_name = wlink
host_OS = dos
host_CPU = i86

!include cproj.mif
!include deftarg.mif

template = &
fcenable.$(ext) &
mem.$(ext) &
records.$(ext) &
wildargv.$(ext)

ext = obj
os2_objs = $+$(template)$-

ext = dbj
dos_objs = $+$(template)$-
.extensions:
.extensions: .exe .obj .dbj .c

copts   = /fpc/oaxt /ms /d1 /w4 /we

!ifdef on_build_machine
copts += -zq
!endif

compile = $(cc_i86) $[* $(copts)
proj = fcenable
dosproj = fcdos

prj = fcenable.exe

.c: ..\c;$(lang_root)\src\startup

.c.obj:
        $(compile) /bt=os2

.c.dbj:
        $(compile) /bt=dos /fo=.dbj

$(prj) : $(os2_objs) spawn.obj $(dosproj).exe
        $(linker) system os2 pmcompat n $(prj) op map f {spawn $(os2_objs)} op stub=$(dosproj).exe

$(dosproj).exe : $(dos_objs)
        $(linker) system dos n $(dosproj) op map f {$(dos_objs)}

spawn.obj : spawn.c
        $(compile) /i=$(lang_root)\h\os21x /bt=os2
