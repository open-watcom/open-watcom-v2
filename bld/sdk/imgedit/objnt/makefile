targflags = -mf-bt=NT
comp = $(comp386)
extras = wincreat.obj desknt.obj title.obj winutils.obj
rc = $(wrce)
resflags =-zk0 -r -d__NT__ -bt=NT -fo=..\objnt\$(name).res $(resdir)$(name).rc
wr_dir=$(sdk_dir)\wr
wres_dir=$(sdk_dir)\rc\wres

!include ..\imgedit.mif

.before
        @set wcc386=-i=$(sdk_dir)\misc;$(watcom_nt_h)
        @set INCLUDE=.\h;..\h;$(wres_dir)\h;$(wr_dir)\h;$(wpi_dir)\h;$(lang_root)\h;$(watcom_nt_h);$(watcom_h);
        @set LIB=$(wr_dir)\nt;$(wpi_dir)\lib;$(sdk_dir)\rc\wres\lib;$(%LIB)

$(name).res : $(rcdeps)
    $(RC) $(resflags) -i=..\res;..\..\misc

$(lnk) : makefile ..\imgedit.mif
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) system nt_win
        @%append $(lnk) libpath $(sdk_dir)rc\wres\lib
        @%append $(lnk) library nt
        @%append $(lnk) library wresfnt.lib
        @%append $(lnk) library wpi_nt.lib
        @%append $(lnk) library wr.lib
        @%append $(lnk) library ..\..\misc\ctl3d32.lib
        @%append $(lnk) name $(name).exe
        @%append $(lnk) option map
        @%append $(lnk) option stack=32k
        @for %i in ($(objs)) do @%append $(lnk) file %i

$(msgh) : $(mastermsg)
        vi -i -q "-k :source ..\\\\..\\\\misc\\\\msgtoh.vi $(msgh)\n" &
        $(mastermsg)
        -del about.obj

$(mastermsg) : $(msgfiles)
        $(comp386) -zk0 -bt=NT -p -i=..\..\misc ..\h\wimgedit.msg > $(mastermsg)

