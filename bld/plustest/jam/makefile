plustest_name = jam

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -otexan-d1
alt_error=00
!endif

test_flags += -w0 -I..

test_flags_386 = -mf
test_flags_i86 = -mm
test_flags_axp =

!include testenv1.mif

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

test : .symbolic start clean start_test test.lnk testftor.$(exe) common_set_result
    @%make clean

testftor.$(exe) : testftor.obj assert.obj

start_test : .symbolic
    @%make common_start_test

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
