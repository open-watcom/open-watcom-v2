name = pbide
lnk = $(name).lnk

cflags  = -bt=windows -zW -bd -zq -otexan -s -zq -fpc -w4 -ml -zu -fo=obj\.obj
!ifndef on_build_machine
cflags += -d2
!endif

driver = driver
dlnk = $(driver).lnk
dobj = driver.obj

$(driver).exe : $(dobj) $(dlnk) driver.res $(name).lib
        wlink @$(dlnk)
        wrc -k driver.res $(driver).exe

$(name).lib: ..\owindll\$(name).dll
        wtouch $^@
        rm $^@
        wlib $^@ +$[@
        copy $[@

$(dlnk) : makefile
        %create $(dlnk)
        @%append $(dlnk) debug all
        @%append $(dlnk) sys windows
        @%append $(dlnk) op map
        @%append $(dlnk) libr $(name).lib
        @%append $(dlnk) name $(driver).exe
        @for %i in ($(dobj)) do @%append $(dlnk) file obj\%i

driver.res : driver.rc driver.dlg
        wrc -r -bt=windows driver.rc

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
