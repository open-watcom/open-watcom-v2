cflags= -ml-zq-2-s-zu-zW-fpc-w4-i=$(sdk_misc_dir) -fo=$[&.obj
resflags = -i=$(sdk_misc_dir);.. -fo=$(name).res
compiler= $(comp286)
os = windows

!include ..\wddespy.mif

$(lnk) : makefile ..\wddespy.mif
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) system windows
        @%append $(lnk) name $(name).exe
        @%append $(lnk) library windows.lib
        @%append $(lnk) library toolhelp.lib
        @%append $(lnk) library ddeml.lib
        @%append $(lnk) library commdlg.lib
        @%append $(lnk) library $(sdk_misc_dir)\ctl3d.lib
        @%append $(lnk) option map
        @%append $(lnk) option stack=7k
        @%append $(lnk) option heapsize=2k
        @for %i in ($(objs)) do @%append $(lnk) file %i

!include ..\rules.mif
