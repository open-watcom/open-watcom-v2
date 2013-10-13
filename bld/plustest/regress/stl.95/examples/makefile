plustest_name = stl95

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
!else ifdef test4
alt_error = 04
test_flags = -oxa-d1-xs
test_flags_386 = $(br_opt)
test_flags_i86 =
test_flags_axp = $(br_opt)
!else ifdef test5
alt_error = 05
test_flags = -d2-xs
test_flags_386 = $(br_opt)
test_flags_i86 =
test_flags_axp = $(br_opt)
!else
alt_error = 00
test_flags = -d2
!endif

test_flags += -w0 -I.. -fi=../pch.h -fh

test_flags_i86 += -mm -zm

extra_run_istmit1 = <istmit1.in
extra_run_tech01 = | sed -f fixpath.sed

stack_386=opt stack=64k
stack_i86=opt stack=16k
stack_axp=opt stack=64k

opt_link = FILE tempbuf.obj, random.obj

.cpp: ../../stl.94/examples;..

!include ../../stl.94/examples/stl_test.mif

test : .symbolic start clean start_test test.lnk tempbuf.obj random.obj $(tests)
!ifeq arch i86
    diff $(%ERROR_FILE) execi86.chk
!else
    diff $(%ERROR_FILE) exec.chk
!endif
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
