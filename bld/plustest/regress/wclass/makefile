plustest_name = wclass

!include ../environ.mif

!ifdef test1
comp_flags_i86 = -d2-ml
comp_flags_386 = -d2-mf
comp_flags_axp = -d2
alt_error=01
!else ifdef test2
comp_flags_i86 = -ox-d1-xds-mc
comp_flags_386 = -ox-d1-xds-mf
comp_flags_axp = -ox-d1-xds
alt_error=02
!else
comp_flags_i86 = -ox-d1-mc
comp_flags_386 = -ox-d1-mf
comp_flags_axp = -ox-d1
alt_error=00
!endif

comp_flags = -zq -I"$(%watcom)/h"

comp_flags_i86 += -zw -I"$(%watcom)/h/win" -bt=windows -DSTRICT
comp_flags_386 += -I"$(%watcom)/h/nt"
comp_flags_axp += -I"$(%watcom)/h/nt"

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

.cpp.obj:
    $(wpp_$(arch)) $[* $(comp_flags) $(comp_flags_$(arch)) $(extra) -fo=.obj

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

!ifeq arch i86
test : .symbolic start_test test.exe
!else ifeq arch 386
!ifeq targ_env_386 wnt386
test : .symbolic start_test test.exe
!else
test : .symbolic start_test
!endif
!else
test : .symbolic start_test
!endif
    @%make global
    %append $(log_file) PASS $(%__CWD__)

test.exe : $(objs) test.lnk
    $(linker) @$]@ NAME $@

test.lnk : makefile $(objs)
    %create $^@
!ifeq arch i86
    @%append $^@ SYSTEM windows
    @%append $^@ OPTION rwreloccheck
    @%append $^@ LIBRARY $(%watcom)/lib286/win/ddeml.lib
    @%append $^@ LIBRARY $(%watcom)/lib286/win/commdlg.lib
!else
    @%append $^@ SYSTEM nt_win
!endif
    @%append $^@ $(ldebug_$(arch))
    @for %i in ( $(objs) ) do @%append $^@ FILE %i

start_test : .symbolic
    %create exec.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .procedure
    @%make common_clean
