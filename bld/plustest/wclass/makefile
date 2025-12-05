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
test : .SYMBOLIC start test.$(exe)
!else ifeq arch 386
!ifeq targ_env_386 wnt386
test : .SYMBOLIC start test.$(exe)
!else
test : .SYMBOLIC start
!endif
!else
test : .SYMBOLIC start
!endif
    @%append $(log_file) PASS $(%__CWD__)
    @%make clean

test.$(exe) : $(objs)
    @%make set_path_wlink
    @%make echo_link
    @%make test1.lnk
    @which $(linker)
    $(noecho)$(linker) @test1.lnk NAME $@
    @%make set_old_path

test1.lnk : makefile $(objs)
    $(noecho)%create test1.lnk
    @%append test1.lnk DEBUG line
!ifndef noisy
    @%append test1.lnk opt quiet
!endif
    @%append test1.lnk $(lnk_any)
    @%append test1.lnk $(stack_$(arch))
!ifeq arch i86
    @%append test1.lnk SYSTEM windows
    @%append test1.lnk OPTION rwreloccheck
    @%append test1.lnk LIBRARY $(%WATCOM)/lib286/win/ddeml.lib
    @%append test1.lnk LIBRARY $(%WATCOM)/lib286/win/commdlg.lib
!else
    @%append test1.lnk SYSTEM nt_win
!endif
    @for %i in ( $(objs) ) do @%append test1.lnk FILE %i

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
