plustest_name = winnt

!include testenv.mif

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

test_flags += -w0 -I"$(%WATCOM)/h/nt"

stack_386 = op stack=128k

!ifndef __NT__
test : .symbolic start clean start_test
!else ifeq arch i86
test : .symbolic start clean start_test
!else ifeq targ_env_386 wnt386
test : .symbolic start clean start_test tomdll.dll test.lnk tomexe.$(exe)
    cat tomexe.out >>$(%ERROR_FILE)
    diff $(%ERROR_FILE) exec.chk
!else ifdef __NT386__
test : .symbolic start clean start_test
!else
test : .symbolic start clean start_test tomdll.dll test.lnk tomexe.$(exe)
    cat tomexe.out >>$(%ERROR_FILE)
    diff $(%ERROR_FILE) exec.chk
!endif
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

tomdll.dll : tomdll.obj tomdll.lnk
    @%make set_path_wlink
    $(linker) @$]@ NAME $@
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

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
