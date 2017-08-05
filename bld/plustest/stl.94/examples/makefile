plustest_name = stl94

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
!else
alt_error = 00
test_flags = -d2
!endif

test_flags += -w0 -I".;../source" -fi=../source/pch.h -fhq

test_flags_i86 = -mm -zm

extra_run_istmit1 = <istmit1.in

stack_386=opt stack=64k
stack_i86=opt stack=16k
stack_axp=opt stack=64k

opt_link = FILE tempbuf.obj, random.obj
.cpp: .;../source

!include stl_test.mif

test : .symbolic start clean start_test test.lnk tempbuf.obj random.obj $(tests)
    @set ERROR_FILE=$(log_file)
    @set ERROR_MSG=FAILED
    $(noecho)%make save_sav
!ifeq arch i86
    diff $(sav_file) execi86.chk
!else
    diff $(sav_file) exec.chk
!endif
    $(noecho)%make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    $(noecho)%make clean_sav
    @set ERROR_FILE=exec.out
    $(noecho)%create $(%ERROR_FILE)

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    $(noecho)%make common_clean
