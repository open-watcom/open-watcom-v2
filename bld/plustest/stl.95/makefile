plustest_name = stl95

!include testenv1.mif

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

test_flags += -w0 -Isource -fi=source/pch.h -fh

test_flags_i86 += -mm -zm

extra_run_istmit1 = <istmit1.in
extra_run_tech01 = | sed -f fixpath.sed

stack_386=opt stack=64k
stack_i86=opt stack=16k
stack_axp=opt stack=64k

opt_link = FILE tempbuf.obj, random.obj

.cpp: ../stl.94/examples;source

!include ../stl.94/examples/stl_test.mif

test : .symbolic start clean start_test test.lnk tempbuf.obj random.obj $(tests) set_result
    @%make clean

set_result : .symbolic
    $(noecho)%make set_fail
    $(noecho)%make common_save_sav
!ifeq arch i86
    diff $(sav_file) execi86.chk
!else
    diff $(sav_file) exec.chk
!endif
    $(noecho)%make common_clean_sav
    $(noecho)%make set_pass

start_test : .symbolic
    @%make common_start_test

clean : .procedure
    $(noecho)%make common_clean
