!include cproj.mif

# CATFILE -- another cat-like utility

.extensions:
.extensions: .exe .lib .lnk .dbj .abj .obj .cpp .c

# optimized = yes

!ifdef optimized
.cpp.obj : .AUTODEPEND
    $(cpp_386) $[* -ew-d1-zm-w4-zq-ox
.c.obj : .AUTODEPEND
    $(cc_386) $[* -d1-w4-zq-ox
.c.dbj : .AUTODEPEND
    $(cc_i86) $[* -d1-w4-zq-ml-fo=*.dbj
.c.abj : .AUTODEPEND
    $(cc_axp) $[* -ox-d1-w4-zq-fo=*.abj
.cpp.abj : .AUTODEPEND
    $(cpp_axp) $[* -ox-ew-d2-w4-zq-fo=*.abj
!else
.cpp.obj : .AUTODEPEND
    $(cpp_386) $[* -ew-d2-zm-bt=os2-w4-zq
.c.obj : .AUTODEPEND
    $(cc_386) $[* -d2-w4-zq
.c.dbj : .AUTODEPEND
    $(cc_i86) $[* -d2-w4-zq-ml-fo=*.dbj
.c.abj : .AUTODEPEND
    $(cc_axp) $[* -d2-w4-zq-fo=*.abj
.cpp.abj : .AUTODEPEND
    $(cpp_axp) $[* -ew-d2-w4-zq-fo=*.abj
!endif

!ifndef busted
catfile.exe : makefile catfile.lnk catfile.obj dosstub.exe
    wlink @catfile.lnk file catfile
!else
catfile.exe : makefile catfile.lnk catfile.obj
    wlink @catfile.lnk file catfile
!endif

catfaxp.exe : makefile catfaxp.lnk catfile.abj
    wlink @catfaxp.lnk file catfile.abj

catfile.lnk : makefile
    @%create  $^@
    @%append $^@ NAME $^*
    @%append $^@ SYSTEM os2v2 pmc
    @%append $^@ OPTION quiet, heap=16k, stub=dosstub.exe
#   @%append $^@ OPTION quiet, heap=16k
    @%append $^@ DEBUG   all

dosstub.exe : catfile.dbj dosstub.lnk
    wlink @dosstub.lnk file catfile.dbj

dosstub.lnk : makefile
    %create $^@
    @%append $^@ NAME dosstub
    @%append $^@ SYSTEM dos
    @%append $^@ OPTION quiet
    @%append $^@ DEBUG  all

catfaxp.lnk : makefile
    %create $^@
    @%append $^@ NAME catfaxp
    @%append $^@ SYSTEM ntaxp
    @%append $^@ OPTION quiet
    @%append $^@ DEBUG  codeview


global: .symbolic
    -del *.err
    -del *.exe
    -del *.dbj
    -del *.obj
    -del *.lib
    -del *.lnk
