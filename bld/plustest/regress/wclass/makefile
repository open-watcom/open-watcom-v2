plustest_name = wclass
!include ..\environ.mif

!ifeq arch 386
!define arch i86
!endif

!ifeq arch i86

!ifdef test1
comp_flags = -d2-ml
!else ifdef test2
comp_flags = -ox-d1-xds-mc
!else
comp_flags = -ox-d1-mc
!endif

comp_flags += -zq -zw -i$(%watcom)\h\win -bt=windows

.cpp.obj:
    $(wpp_i86) $[* $(comp_flags) $(extra)
    
objs = &
nullfunc.obj &
wboolsw.obj &
wbrush.obj &
wbutton.obj &
wcheckbx.obj &
wclient.obj &
wcombox.obj &
wcontrol.obj &
wdraw.obj &
weditbox.obj &
wfiledlg.obj &
wfont.obj &
wgroupbx.obj &
wlistbox.obj &
wmenu.obj &
wmenuitm.obj &
wobjmap.obj &
wpalette.obj &
wpen.obj &
wpopmenu.obj &
wport.obj &
wpshbttn.obj &
wradbttn.obj &
wrect.obj &
wserver.obj &
wstring.obj &
wtest.obj &
wtext.obj &
wvcllctn.obj &
wvlist.obj &
wwindow.obj &
wtimer.obj &
wmdiwndw.obj &
wmdichld.obj &
wmsgdlg.obj &
wwinmain.obj

test : .symbolic test.exe
    copy test.exe test.ice
    %make global
    %append $(log_file) PASS $(%__CWD__)
    ren test.ice test.exe

test.exe : $(objs) test.lnk
    $(linker) @$]@

test.lnk : makefile $(objs)
    %create $^@
    @%append $^@ SYSTEM windows
    @%append $^@ OPTION map, quiet, rwreloccheck
    @%append $^@ NAME $^*
    @%append $^@ DEBUG all
    @for %i in ( $(objs) ) do @%append $^@ FILE %i
    @%append $^@ LIBRARY $(%watcom)\lib286\win\DDEML.LIB
    @%append $^@ LIBRARY $(%watcom)\lib286\win\COMMDLG.LIB

!else
    
test : .symbolic
    %make global
    %append $(log_file) PASS $(%__CWD__)

!endif
    
global : .procedure
    %make common_clean
