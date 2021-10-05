##pmake: nobuild guitool os_win cpu_i86

name = pbide

!include cproj.mif
!include deftarg.mif

cflags  = -bt=windows -zW -bd -zq -otexan -s -zq -fpc -w4 -ml -zu -fo=obj\.obj
!ifeq release 0
cflags += -d2
!endif

driver = driver
dobj = driver.obj

driver.exe : $(dobj) driver.res $(name).lib
        $(noecho)%create $^&.lnk
        @%append $^&.lnk debug all
        @%append $^&.lnk sys windows
        @%append $^&.lnk op map
        @%append $^&.lnk libr $(name).lib
        @for %i in ($(dobj)) do @%append $^&.lnk file obj\%i
        wlink name $@ @$^&.lnk
        @%make bind_res

$(name).lib: ..\owindll\$(name).dll
        wtouch $@
        rm $@
        wlib $@ +$[@
        copy $[@


driver.res : driver.rc driver.dlg
        $(rc) $(rc_flags) driver.rc -fo=$@

driver.obj : driver.c
        wcc $[* -bt=windows  -d2 -zW -zq -zq -fpc -w4 -ml -fo=obj\.obj

.obj : obj
.c : .

.c.obj :
        wcc $[* $(cflags)
