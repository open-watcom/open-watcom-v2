targflags = -zW-bt=OS2_PM
# CC = wcc386.exe -d2 -omilert -s -w4 -D_Optlink=
rc = f:\toolkt21\os2bin\rc res\$(name).rc
resflags =-r > nul
comp = $(comp386)
# extras =

!include ..\imgedit2.mif

.before
        @set wcc=-i=%os2drv%\toolkt21\c\os2h
        @set INCLUDE=.\h;..\h;..\..\misc;$(wpi_dir)\h;$(%toolkit)\c\os2h;$(wat_utils)\h;$(lang_root)\h;$(%include)
        @set LIB=$(wpi_dir)\lib;$(%lib)

$(name).res : $(rcdeps)
        @$(%toolkit)\os2bin\rc -r -DBMP_DIR=$(sdk_dir)\wimgedit\res -DMISC_DIR=$(sdk_dir)\misc -I .\res res\$(name).rc $(name).res

$(lnk) : makefile ..\imgedit2.mif
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) system os2v2_pm
        @%append $(lnk) name $(name).exe
        @%append $(lnk) library wpi_os2.lib
        @%append $(lnk) option symfile=$(name).sym
        @%append $(lnk) option map
        @%append $(lnk) option stack=12k
        @%append $(lnk) option heapsize=2k
        @for %i in ($(objs)) do @%append $(lnk) file %i
