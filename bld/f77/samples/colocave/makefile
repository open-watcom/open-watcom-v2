
# Makefile for Open Watcom Fortran

# Use unref=1 on command line to get not 
# referenced warnings & extensions used messages

exeext=.exe     # all platforms currently supported?
cplnk=wfl386    # originally f77
flags=          # originally -o

# These warnings are of no consequence to the application
!ifndef unref
flags+= -NOEX -NOR
!endif

all: asetup$(exeext) adventure$(exeext) .symbolic
    asetup$(exeext)  # builds advtxt from advdat

asetup$(exeext): asetup.for asubs.for
    $(cplnk) $(flags) -FE=asetup$(exeext) asetup.for asubs.for

adventure$(exeext): aamain.for asubs.for
    $(cplnk) $(flags) -FE=adventure$(exeext) aamain.for asubs.for

clean: .symbolic
    @rm *.obj
    @rm *.err
    @rm *.exe
    @rm advtxt*
    @echo Clean performed.
