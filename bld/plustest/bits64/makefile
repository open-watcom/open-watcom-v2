plustest_name = bits64

!include testenv1.mif

!ifdef test1
test_flags_386 = -d2-3r
test_flags_i86 = -d2
test_flags_axp = -d2
alt_error=01
!else
test_flags_386 = -ox-d1-3s
test_flags_i86 = -ox-d1
test_flags_axp = -ox-d1
alt_error=00
!endif

test_flags_386 += -mf
test_flags_i86 += -ml
test_flags_axp +=

test_flags = -d_USE_INT64

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

.cpp: ../bits

test : .symbolic start clean start_test test.lnk tbits.$(exe) set_result
    @%make clean

set_result : .symbolic
    $(noecho)%make set_fail
    $(noecho)%make common_save_sav
    diff $(sav_file) ../bits/exec.chk >$(nulldevice)
    $(noecho)%make common_clean_sav
    $(noecho)%make set_pass

start_test : .symbolic
    @%make common_start_test

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
