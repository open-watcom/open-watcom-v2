cflags= -mf-zq-3-s-fpc-w4-bt=nt-i=$(sdk_misc_dir) -i=$(watcom_nt_h) -fo=$[&.obj
resflags = -bt=nt -i=$(sdk_misc_dir);..;$(watcom_nt_h) -fo=$(name).res
compiler= $(comp386)
os = nt

!include ..\wddespy.mif

$(lnk) : makefile ..\wddespy.mif
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) system nt_win
        @%append $(lnk) name $(name).exe
        @%append $(lnk) library $(sdk_misc_dir)\ctl3d32.lib
        @%append $(lnk) option map
        @for %i in ($(objs)) do @%append $(lnk) file %i

!include ..\rules.mif
