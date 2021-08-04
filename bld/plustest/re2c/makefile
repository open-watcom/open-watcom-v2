plustest_name = re2c

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags_386 = -mf-3s
test_flags_i86 = -ml -3
test_flags_axp =

sav_file_2 = x$(arch)_$(alt_error).sav

!include testenv.mif

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

extra_run_re2c = -d plusplus.lex

test : .symbolic start test.lnk re2c.$(exe) rename_result_file
    $(noecho)%make common_set_result_check

rename_result_file : .procedure
    @if exist exec2.out @rm exec2.out
    %ren debug.out exec2.out

check_result : .procedure
    -diff $(sav_file) exec.chk
    -diff $(sav_file_2) exec2.chk | wc >wc.out
    -diff -b wc.out wc.chk

re2c.$(exe) : re2c.obj code.obj dfa.obj parser.obj scanner.obj useful.obj asc2ebc.obj ebc2asc.obj getopt.obj

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
