# PPACT -- pretty-printer for code generator actions file

.extensions:
.extensions: .exe .lib .lnk .obj .cpp

TRPRTDLL  = yes
optimized = yes

.cpp.obj : .AUTODEPEND
!ifdef optimized
!   ifdef TRPRTDLL
        $(comp386plus) $[* -ew-d1-zm-bt=os2-w4-zq-hd-ox -dTRPRTDLL -i"../test/trpr"
!   else
        $(comp386plus) $[* -ew-d2-zm-bt=os2-w4-zq
!   endif
!else
!   ifdef TRPRTDLL
        $(comp386plus) $[* -ew-d2-bt=os2-w4-zq-hd -dTRPRTDLL -i"../test/trpr"
!   else
        $(comp386plus) $[* -ew-d2-zm-bt=os2-w4-zq
!   endif
!endif

object_files =
!ifndef TRPRTDLL
object_files +=  setimpl.obj

ppact.wlb : makefile
    @%write $^@
    @for %i in ($(object_files)) do @%append $^@ + %i

ppact.lib : $(object_files) ppact.wlb
    @rm -f ppact.lib
    wlib -q -n ppact.lib @ppact.wlb
!endif

ppact.lnk : makefile
    @%create  $^@
    @%append $^@ SYSTEM  os2v2
    @%append $^@ OPTION  quiet, eliminate, map, show, caseexact
    @%append $^@ DEBUG   dwarf

!ifdef TRPRTDLL
ppact.exe : makefile ppact.lnk ppact.obj
!else
ppact.exe : makefile ppact.lnk ppact.obj ppact.lib
!endif
    wlink @ppact.lnk name ppact file ppact


global: .symbolic
    -del *.err
    -del *.exe
    -del *.obj
    -del *.lib
    -del *.lnk
