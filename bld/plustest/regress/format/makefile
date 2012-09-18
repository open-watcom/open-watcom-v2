plustest_name = format

!include testenv.mif

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

testi.$(exe) : testi.obj iformat.obj
testf.$(exe) : testf.obj fformat.obj

test : .symbolic clean start_test test.lnk $(exes)
    diff exec.out exec.chk
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
