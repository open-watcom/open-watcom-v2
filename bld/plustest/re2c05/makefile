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

test : .symbolic start clean start_test test.lnk re2c.$(exe)
    %make save_sav
    -diff $(sav_file) debug.chk | wc >wc.out
    diff -b wc.out wc.chk
    %make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

re2c.$(exe) : re2c.obj code.obj dfa.obj memchk.obj parser.obj scanner.obj substr.obj translat.obj actions.obj

start_test : .symbolic
    %make clean_sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    %make common_clean
