#
# Additional extensions
#
.EXTENSIONS : .ih .idl .res .rc

#
# Compiler flags
#
CFLAGS = -zq -bd -sg -I.
SCFLAGS = -maddstar
!ifdef debug
CFLAGS += -d2
LFLAGS += debug all
!endif

#
# Default/Inference rules
#
.c.obj:
    wcc386 $(CFLAGS) $[@

.idl.ih :
    sc -s"ih" $(SCFLAGS) $<

.idl.h :
    sc -s"h" $(SCFLAGS) $< 

.idl.c :
    sc -s"c" $(SCFLAGS) $< 

.rc.res :
    wrc -r -q $[*

#
# Pseudotargets
#

help: .symbolic
    @echo.
    @echo Open Watcom Sample WPS Class makefile
    @echo.
    @echo The following pseudotargets are supported:
    @echo.
    @echo  all       - builds the WPS class DLL
    @echo  install   - builds the WPS class DLL, installs it and creates a test object
    @echo  deinstall - removes the test object and deinstalls the WPS class DLL
    @echo  clean     - executes deinstall and removes compiled files

all: owobject.dll .symbolic

install: all .symbolic
    -@call run install

deinstall: .symbolic
    -@call run deinstall

clean : .symbolic
    @echo cleaning up
    -@call run deinstall -q
    @rm -f *.obj *.dll owobject.h *.ih *.err *.res *.map

# Dependencies with explicit commands

OBJS = owobject.obj nbpage.obj helper.obj
owobject.dll : $(OBJS) owobject.lnk owobject.res
    wlink @owobject $(LFLAGS) op map file { $(OBJS) }
    wrc -q $*.res $@

# Dependencies for inference rules (using implicit commands)

owobject.res: owobject.rc owobject.dlg owobject.ico

owobject.ih owobject.h owobject.c: owobject.idl

owobject.obj : owobject.c owobject.ih owobject.h nbpage.h helper.h

nbpage.obj : nbpage.c nbpage.h

helper.obj : helper.c helper.h

