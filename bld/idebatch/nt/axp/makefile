#pmake: os_nt cpu_axp

host_os  = nt
host_cpu = axp

OBJECTS = batserv.obj batpipe.obj
TESTOBJ = tst.obj

.mif : $(mif_path)
.c : ..;../../os2

!include cproj.mif
!include defrule.mif
!include deftarg.mif

inc_dirs = -I"../../h"

extra_l_flags = op stack=32K

batserv.exe : $(OBJECTS)
        $(linker) $(lflags) name batserv file {$(OBJECTS)}

all : batserv.exe batcli.obj batpipe.obj tst.exe .symbolic

tst.exe : $(OBJECTS) $(TESTOBJ)
        $(linker) $(lflags) name tst file {$(OBJECTS) $(TESTOBJ)}
