.extensions:
.extensions: .exe .obj .c

!ifndef SYSTEM
!ifdef __OS2__
SYSTEM = OS2V2
!else
SYSTEM = DOS4G
!endif
!endif

compile = wcc386 /i=..\h;$(dwarfdir)\dr\h;$(dwarfdir)\dw\h

.c : $(dwarfdir)\util\c
.c.obj :
        $(compile) $[@ -oaxt-d1-w4

all : $(%devdir)\bin\dump.exe $(%devdir)\bin\hexit.exe $(%devdir)\bin\build.exe
    wtouch all

$(%devdir)\bin\dump.exe : main.obj testdump.obj
    @set OBJS=$<
    @set NAME=$^@
    @%make dolink

$(%devdir)\bin\hexit.exe : hexit.obj
    @set OBJS=$<
    @set NAME=$^@
    @%make dolink

$(%devdir)\bin\build.exe : build.obj
    @set OBJS=$<
    @set NAME=$^@
    @%make dolink

dolink : .PROCEDURE
    wlink system $(SYSTEM) debug all f { $(%OBJS) } name $(%NAME) op map

clean : .symbolic
        rm -f *.obj *.exe *.map *.lib *.lbc *.err *.i
