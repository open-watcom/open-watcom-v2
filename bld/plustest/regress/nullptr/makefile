plustest_name = nullptr

!include testenv.mif

exes = nullptr.$(exe)

!ifdef test1
test_flags += -ms
alt_error=01
!else ifdef test2
test_flags += -mm
alt_error=02
!else ifdef test3
test_flags += -mc
alt_error=03
!else ifdef test4
test_flags += -ml
alt_error=04
!else ifdef test5
test_flags += -mh
alt_error=05
!else
alt_error=00
!endif


stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

test : .symbolic clean start_test test.lnk $(exes)
    diff exec.out nullptr.chk
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
