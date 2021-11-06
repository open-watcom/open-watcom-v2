plustest_name = stl94

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

test_flags += -w0 -I".;../source" -fi=../source/pch.h -fhq

test_flags_i86 = -mm -zm

exec_chk_i86 = execi86.chk

!include testenv.mif

extra_run_istmit1 = <istmit1.in

stack_386=opt stack=64k
stack_i86=opt stack=16k
stack_axp=opt stack=64k

opt_link = FILE tempbuf.obj, random.obj

.cpp: .;../source

!include stl_test.mif

test : .symbolic start test.lnk tempbuf.obj random.obj $(tests)
    $(noecho)%make common_set_result_display

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
