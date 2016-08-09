# wmakept -- load DLL's and transfer to WMAKE

.before
    set INCLUDE=$(%INCLUDE);$(watcom_dir)\os220_h


.extensions:
.extensions: .exe .lib .lnk .obj .cpp .c

#optimized = yes

!ifdef optimized
.cpp.obj : .AUTODEPEND
    $(comp386plus) $[* -ew-d1-zm-bt=os2-w4-zq-ox
.c.obj : .AUTODEPEND
    $(comp386) $[* -d1-bt=os2-w4-zq-ox
!else
.cpp.obj : .AUTODEPEND
    $(comp386plus) $[* -ew-d2-zm-bt=os2-w4-zq
.c.obj : .AUTODEPEND
    $(comp386) $[* -d2-bt=os2-w4-zq
!endif

wmakept.lnk : makefile
    @%create  $^@
    @%append $^@ SYSTEM  os2v2
    @%append $^@ OPTION  caseexact, quiet, eliminate, map, show
    @%append $^@ OPTION  dosseg, stack=64k, offset=64k, newfiles
    @%append $^@ DEBUG   all

wmakept.exe : wmakept.lnk wmakept.obj
    wlink @wmakept.lnk name wmakept file wmakept


global: .symbolic
    -del *.err
    -del *.exe
    -del *.obj
    -del *.lnk
