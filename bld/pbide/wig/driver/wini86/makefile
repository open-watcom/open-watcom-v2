##pmake: nobuild os_win cpu_i86

name = pbide
lnk = $(name).lnk

!include cproj.mif
!include deftarg.mif

cflags  = -bt=windows -zW -bd -zq -otexan -s -zq -fpc -w4 -ml -zu -fo=obj\.obj
!ifeq release 0
cflags += -d2
!endif

driver = driver
dlnk = $(driver).lnk
dobj = driver.obj

$(driver).exe : $(dobj) $(dlnk) driver.res $(name).lib
        wlink @$(dlnk)
        $(rc) -k driver.res $^@

$(name).lib: ..\owindll\$(name).dll
        wtouch $^@
        rm $^@
        wlib $^@ +$[@
        cp $[@

$(dlnk) : makefile
        %create $(dlnk)
        @%append $(dlnk) debug all
        @%append $(dlnk) sys windows
        @%append $(dlnk) op map
        @%append $(dlnk) libr $(name).lib
        @%append $(dlnk) name $(driver).exe
        @for %i in ($(dobj)) do @%append $(dlnk) file obj\%i

driver.res : driver.rc driver.dlg
        $(rc) $(rc_flags) driver.rc -fo=$^@

driver.obj : driver.c
        wcc $[* -bt=windows  -d2 -zW -zq -zq -fpc -w4 -ml -fo=obj\.obj

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .asm .c .h

.obj : obj
.c : .

.c.obj :
        wcc $[* $(cflags)
