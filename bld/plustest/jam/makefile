plustest_name = jam

!include testenv1.mif

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -otexan-d1
alt_error=00
!endif

test_flags += -w0-I..

test_flags_386 += $(test_flags) -mf
test_flags_i86 += $(test_flags) -mm
test_flags_axp += $(test_flags)

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

test : .symbolic start clean start_test test.lnk testftor.$(exe)
    %make save_sav
    diff $(sav_file) jam.chk
    %make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

testftor.$(exe) : testftor.obj assert.obj

start_test : .symbolic
    %make clean_sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    %make common_clean
