plustest_name = re2c

!include testenv1.mif

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags_386 += $(test_flags) -mf-3s
test_flags_i86 += $(test_flags) -ml -3
test_flags_axp += $(test_flags)

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

extra_run_re2c = -d plusplus.lex

sav_file_2 = x$(arch)_$(alt_error).sav

test : .symbolic start clean start_test test.lnk re2c.$(exe)
    %make save_sav
    -diff $(sav_file) exec.chk
    -diff $(sav_file_2) debug.chk | wc >wc.out
    diff -b wc.out wc.chk
    %make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

re2c.$(exe) : re2c.obj code.obj dfa.obj parser.obj scanner.obj useful.obj asc2ebc.obj ebc2asc.obj getopt.obj

start_test : .symbolic
    %make clean_sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist debug.out @cp debug.out $(sav_file_2) >$(nulldevice)
    @if exist exec.out @rm exec.out
    @if exist debug.out @rm debug.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)
    @if exist $(sav_file_2) @rm $(sav_file_2)

clean : .procedure
    %make common_clean
