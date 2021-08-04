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
test : .symbolic start
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean
!else ifeq arch i86
test : .symbolic start
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean
!else ifeq targ_env_386 wnt386
test : .symbolic start tomdll.dll test.lnk tomexe.$(exe) test_set_result
    $(noecho)%make common_set_result_display
!else ifdef __NT386__
test : .symbolic start
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean
!else
test : .symbolic start tomdll.dll test.lnk tomexe.$(exe) test_set_result
    $(noecho)%make common_set_result_display
!endif

test_set_result : .symbolic
    cat tom.out >>$(%ERROR_FILE)

tomdll.dll : tomdll.obj tomdll.lnk
    @%make set_path_wlink
    @%make echo_link
    $(noecho)$(linker) @$]@ NAME $@
    @%make set_old_path

tomdll.obj : tomdll.c
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ $(test_flags) $(test_flags_$(arch)) -bd
    @%make set_old_path

tomdll.lnk : $(__MAKEFILES__)
    $(noecho)%create $@
    @%append $@ $(ldebug_$(arch))
    @%append $@ $(lnk_any)
    @%append $@ SYSTEM nt dll initinstance terminstance reference __DLLstart_
    @%append $@ OPTION map, caseexact, stack=128k, quiet
    @%append $@ FILE tomdll.obj

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
