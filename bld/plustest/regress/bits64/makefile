plustest_name = bits64

!include testenv.mif

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

test : .symbolic start clean start_test test.lnk tbits.$(exe)
    diff $(%ERROR_FILE) ../bits/tbits.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
