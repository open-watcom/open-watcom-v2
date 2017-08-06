plustest_name = re2c05

!include testenv1.mif

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags_386 += $(test_flags) -mf-zc
test_flags_i86 += $(test_flags) -ml-zc
test_flags_axp += $(test_flags)

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

extra_run_re2c = -b scanner.re

test : .symbolic start clean start_test test.lnk re2c.$(exe) set_result
    @%make clean

set_result : .symbolic
    $(noecho)%make set_fail
    $(noecho)%make common_save_sav
    -diff $(sav_file) debug.chk | wc >wc.out
    diff -b wc.out wc.chk
    $(noecho)%make common_clean_sav
    $(noecho)%make set_pass

re2c.$(exe) : re2c.obj code.obj dfa.obj memchk.obj parser.obj scanner.obj substr.obj translat.obj actions.obj

start_test : .symbolic
    @%make common_start_test

clean : .procedure
    $(noecho)%make common_clean
