plustest_name = format

!include testenv1.mif

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

test : .symbolic start clean start_test test.lnk $(exes)
    @set ERROR_FILE=$(log_file)
    @set ERROR_MSG=FAILED
    $(noecho)%make save_sav
    diff $(sav_file) exec.chk
    $(noecho)%make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

testi.$(exe) : testi.obj iformat.obj
testf.$(exe) : testf.obj fformat.obj

start_test : .symbolic
    $(noecho)%make clean_sav
    @set ERROR_FILE=exec.out
    $(noecho)%create $(%ERROR_FILE)

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    $(noecho)%make common_clean
