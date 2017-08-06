plustest_name = format

!include testenv1.mif

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags_axp = $(test_flags)
test_flags_386 = $(test_flags) -mf
test_flags_i86 = $(test_flags) -mc

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

exes = testi.$(exe) testf.$(exe)

test : .symbolic start clean start_test test.lnk $(exes) set_result
    @%make clean

set_result : .symbolic
    $(noecho)%make common_set_result

testi.$(exe) : testi.obj iformat.obj
testf.$(exe) : testf.obj fformat.obj

start_test : .symbolic
    @%make common_start_test

clean : .procedure
    $(noecho)%make common_clean
