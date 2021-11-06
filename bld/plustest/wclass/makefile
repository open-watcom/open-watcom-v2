plustest_name = wclass

!ifdef test1
test_flags_i86 = -d2-ml
test_flags_386 = -d2-mf
test_flags_axp = -d2
alt_error=01
!else ifdef test2
test_flags_i86 = -ox-d1-xds-mc
test_flags_386 = -ox-d1-xds-mf
test_flags_axp = -ox-d1-xds
alt_error=02
!else
test_flags_i86 = -ox-d1-mc
test_flags_386 = -ox-d1-mf
test_flags_axp = -ox-d1
alt_error=00
!endif

test_flags = -zq -I"$(%WATCOM)/h"

test_flags_i86 += -zw -bt=windows -DSTRICT

!include testenv.mif

test_flags_i86 += $(incl_sys_win)
test_flags_386 += $(incl_sys_nt)
test_flags_axp += $(incl_sys_nt)

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

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
test : .symbolic start test.$(exe)
!else ifeq arch 386
!ifeq targ_env_386 wnt386
test : .symbolic start test.$(exe)
!else
test : .symbolic start
!endif
!else
test : .symbolic start
!endif
    @%append $(log_file) PASS $(%__CWD__)
    @%make clean

test.$(exe) : $(objs) test1.lnk
    @%make set_path_wlink
    @%make echo_link
    $(noecho)$(linker) @$]@ NAME $@
    @%make set_old_path

test1.lnk : makefile $(objs)
    $(noecho)%create $@
    @%append $@ DEBUG line
!ifndef noisy
    @%append $@ opt quiet
!endif
    @%append $@ $(lnk_any)
    @%append $@ $(stack_$(arch))
!ifeq arch i86
    @%append $@ SYSTEM windows
    @%append $@ OPTION rwreloccheck
    @%append $@ LIBRARY $(%WATCOM)/lib286/win/ddeml.lib
    @%append $@ LIBRARY $(%WATCOM)/lib286/win/commdlg.lib
!else
    @%append $@ SYSTEM nt_win
!endif
    @for %i in ( $(objs) ) do @%append $@ FILE %i

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
