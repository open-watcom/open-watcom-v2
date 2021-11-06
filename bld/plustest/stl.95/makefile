plustest_name = stl95

!ifdef test1
alt_error = 01
test_flags = -oxa-d1
test_flags_i86 = -mm -zm
!else ifdef test2
alt_error = 02
test_flags = -oxa-d1-xs
test_flags_i86 = -mm -zm
!else ifdef test3
alt_error = 03
test_flags = -d2-xs
test_flags_i86 = -mm -zm
!else ifdef test4
alt_error = 04
test_flags = -oxa-d1-xs
test_flags_386 =
test_flags_i86 = -mm -zm
test_flags_axp =
!else ifdef test5
alt_error = 05
test_flags = -d2-xs
test_flags_386 =
test_flags_i86 = -mm -zm
test_flags_axp =
!else
alt_error = 00
test_flags = -d2
test_flags_i86 = -mm -zm
!endif

test_flags += -w0 -Isource -fi=source/pch.h -fh

exec_chk_i86 = execi86.chk

!include testenv.mif

!ifeq alt_error 04
test_flags += $(br_opt)
!else ifeq alt_error 05
test_flags += $(br_opt)
!endif

extra_run_istmit1 = <istmit1.in
extra_run_tech01 = | sed -f fixpath.sed

stack_386=opt stack=64k
stack_i86=opt stack=16k
stack_axp=opt stack=64k

opt_link = FILE tempbuf.obj, random.obj

.cpp: ../stl.94/examples;source

!include ../stl.94/examples/stl_test.mif

test : .symbolic start test.lnk tempbuf.obj random.obj $(tests)
    $(noecho)%make common_set_result_display

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
