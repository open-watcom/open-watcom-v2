plustest_name = re2c05

!include testenv.mif

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

test : .symbolic start clean start_test test.lnk re2c.$(exe)
    diff $(%ERROR_FILE) debug.chk | wc >wc.out
    diff -b wc.out wc.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

re2c.$(exe) : re2c.obj code.obj dfa.obj memchk.obj parser.obj scanner.obj substr.obj translat.obj actions.obj

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @if exist x$(arch)_$(alt_error).sav rm x$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
