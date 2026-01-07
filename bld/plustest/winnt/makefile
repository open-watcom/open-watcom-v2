plustest_name = winnt

!ifdef test1
test_flags = -oxa-d1
alt_error=01
!else ifdef test2
test_flags = -oxa-d1
alt_error=02
!else ifdef test3
test_flags = -d2
alt_error=03
!else
test_flags = -d2
alt_error=00
!endif

test_flags += -w0

!include testenv.mif

test_flags += $(incl_sys_nt)

!ifeq alt_error 02
test_flags += $(br_opt)
!else ifeq alt_error 03
test_flags += $(br_opt)
!endif

stack_386 = op stack=128k

!ifndef __NT__
test : .SYMBOLIC start
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean
!else ifeq arch i86
test : .SYMBOLIC start
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean
!else ifeq targ_env_386 wnt386
test : .SYMBOLIC start tomdll.dll tomexe.$(exe) test_set_result
    $(noecho)%make common_set_result_display
!else ifdef __NT386__
test : .SYMBOLIC start
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean
!else
test : .SYMBOLIC start tomdll.dll tomexe.$(exe) test_set_result
    $(noecho)%make common_set_result_display
!endif

test_set_result : .SYMBOLIC
    cat tom.out >>$(%ERROR_FILE)

tomdll.dll : tomdll.obj
    @%make set_path_wlink
    @%make echo_link
    @%make tomdll.lnk
    @which $(linker)
    $(noecho)$(linker) @tomdll.lnk NAME $@
    @%make set_old_path

tomdll.obj : tomdll.c
    @%make set_path_wpp
    @%make echo_cxx
    @which $(comp_wpp_$(arch))
    $(noecho)$(wpp_$(arch)) $[@ $(test_flags) $(test_flags_$(arch)) -bd
    @%make set_old_path

tomdll.lnk : $(__MAKEFILES__)
    $(noecho)%create tomdll.lnk
    @%append tomdll.lnk $(ldebug_$(arch))
    @%append tomdll.lnk $(lnk_any)
    @%append tomdll.lnk SYSTEM nt dll initinstance terminstance reference __DLLstart_
    @%append tomdll.lnk OPTION map, caseexact, stack=128k, quiet
    @%append tomdll.lnk FILE tomdll.obj

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
