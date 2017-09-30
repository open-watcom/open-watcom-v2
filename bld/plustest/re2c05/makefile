plustest_name = re2c05

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags_386 = -mf-zc
test_flags_i86 = -ml-zc
test_flags_axp =

!include testenv1.mif

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

extra_run_re2c = -b scanner.re

test : .symbolic start clean start_test test.lnk re2c.$(exe) common_set_result_check
    @%make clean

check_result : .procedure
    -diff $(sav_file) exec.chk | wc >wc.out
    -diff -b wc.out wc.chk

re2c.$(exe) : re2c.obj code.obj dfa.obj memchk.obj parser.obj scanner.obj substr.obj translat.obj actions.obj

start_test : .symbolic
    @%make common_start_test

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
