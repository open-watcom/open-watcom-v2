# FAKEIDE -- fakeide for testing

.extensions:
.extensions: .exe .lib .lnk .obj .cpp .c

#optimized = yes

!ifndef SYSTEM
SYSTEM = os2v2
!endif
!ifndef PROJECT
PROJECT = wcpp386d
!endif

!ifdef optimized
.cpp.obj : .AUTODEPEND
    $(comp386plus) $[* -ew-d1-zm-bt=$(SYSTEM)-w4-zq-ox
.c.obj : .AUTODEPEND
    $(comp386) $[* -d1-bt=$(SYSTEM)-w4-zq-ox
!else
.cpp.obj : .AUTODEPEND
    $(comp386plus) $[* -ew-d2-zm-bt=$(SYSTEM)-w4-zq
.c.obj : .AUTODEPEND
    $(comp386) $[* -d2-bt=$(SYSTEM)-w4-zq
!endif

fakeide.exe : fakeide.lnk fakeide.obj consread.obj
    wlink @fakeide.lnk name fakeide file fakeide file consread

fakeide.lnk : makefile
    @%create  $^@
    @%append $^@ SYSTEM  $(SYSTEM)
    @%append $^@ OPTION  caseexact, quiet, eliminate, map, show
    @%append $^@ OPTION  stack=64k, offset=64k
    @%append $^@ DEBUG   all
    @%append $^@ LIBRARY $(PROJECT).lib


global: .symbolic
    -del *.err
    -del *.exe
    -del *.obj
    -del *.lnk
