plustest_name = claslist

!include testenv1.mif

!ifdef test1
test_flags = -d2~
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags_386 = -mf
test_flags_i86 = -mc
test_flags_axp =

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

test : .symbolic start clean start_test test.lnk pcfistt.$(exe) set_result
    @%make clean

set_result : .symbolic
    $(noecho)%make common_set_result

start_test : .symbolic
    @%make common_start_test

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
