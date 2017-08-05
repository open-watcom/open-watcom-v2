plustest_name = winnt

!include testenv1.mif

!ifdef test1
test_flags = -oxa-d1
alt_error=01
!else ifdef test2
test_flags = -oxa-d1 $(br_opt)
alt_error=02
!else ifdef test3
test_flags = -d2 $(br_opt)
alt_error=03
!else
test_flags = -d2
alt_error=00
!endif

test_flags += -w0 $(incl_sys_nt)

stack_386 = op stack=128k

!ifndef __NT__
test : .symbolic start clean start_test
!else ifeq arch i86
test : .symbolic start clean start_test
!else ifeq targ_env_386 wnt386
test : .symbolic start clean start_test tomdll.dll test.lnk tomexe.$(exe)
    cat tomexe.out >>$(%ERROR_FILE)
    %make save_sav
    diff $(sav_file) exec.chk
    %make clean_sav
!else ifdef __NT386__
test : .symbolic start clean start_test
!else
test : .symbolic start clean start_test tomdll.dll test.lnk tomexe.$(exe)
    cat tomexe.out >>$(%ERROR_FILE)
    %make save_sav
    diff $(sav_file) exec.chk
    %make clean_sav
!endif
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    %make clean_sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

tomdll.dll : tomdll.obj tomdll.lnk
    @%make set_path_wlink
    @%make echo_link
    $(noecho)$(linker) @$]@ NAME $@
    @%make set_old_path

tomdll.obj : tomdll.c
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ $(test_flags) -bd
    @%make set_old_path

tomdll.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_any)
    @%append $^@ SYSTEM nt dll initinstance terminstance reference __DLLstart_
    @%append $^@ OPTION map, caseexact, stack=128k, quiet
    @%append $^@ FILE tomdll.obj

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    %make common_clean
