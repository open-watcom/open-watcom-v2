host_OS = nt
host_CPU = axp

OBJECTS = batserv.obj batpipe.obj
TESTOBJ = tst.obj

.mif : $(mif_path)
.c : ..;..\..\os2

!include cproj.mif
!include defrule.mif
!include deftarg.mif

inc_dirs = $(watcom_dir)\h;..\..\h

extra_l_flags = op stack=32K

.before:
        set INCLUDE=$(inc_path);

batserv.exe : $(OBJECTS)
        $(linker) $(lflags) name batserv file {$(OBJECTS)}

all : batserv.exe batcli.obj batpipe.obj tst.exe .symbolic

tst.exe : $(OBJECTS) $(TESTOBJ)
        $(linker) $(lflags) name tst file {$(OBJECTS) $(TESTOBJ)}
