plustest_name = nullptr

!include testenv1.mif

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

test : .symbolic start clean start_test test.lnk $(exes)
    %make save_sav
    diff $(sav_file) nullptr.chk
    %make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

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
