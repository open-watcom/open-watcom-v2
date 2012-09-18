plustest_name = stl94

!include testenv.mif

!ifdef test1
alt_error = 01
test_flags = -oxa-d1
!else ifdef test2
alt_error = 02
test_flags = -oxa-d1-xs
!else ifdef test3
alt_error = 03
test_flags = -d2-xs
!else
alt_error = 00
test_flags = -d2
!endif

test_flags += -w0 -I".;.." -fi=../pch.h -fhq

test_flags_i86 = -mm -zm

extra_run_istmit1 = <istmit1.in

stack_386=opt stack=64k
stack_i86=opt stack=16k
stack_axp=opt stack=64k

opt_link = FILE tempbuf.obj, random.obj
.cpp: .;..

!include stl_test.mif

test : .symbolic clean start_test test.lnk tempbuf.obj random.obj $(tests)
!ifeq arch i86
    diff exec.out execi86.chk
!else
    diff exec.out exec.chk
!endif
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

test.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
