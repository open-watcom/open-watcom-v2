plustest_name = field

!include testenv.mif

!ifdef test1
test_flags_386 = -d2-fpd
test_flags_i86 = -d2-fpd
test_flags_axp = -d2
alt_error=01
!else ifdef test2
test_flags_386 = -os-d1-3s-xds
test_flags_i86 = -os-d1-xds
test_flags_axp = -os-d1-xds
alt_error=02
!else ifdef test3
test_flags_386 = -os-d1-3s-xss-fpd
test_flags_i86 = -os-d1-xss-fpd
test_flags_axp = -os-d1-xss
alt_error=03
!else
test_flags_386 = -os-d1-xst
test_flags_i86 = -os-d1-xst
test_flags_axp = -os-d1-xst
alt_error=00
!endif

test_flags_386 += -fpi87-fp5-mf
test_flags_i86 += -fpi87-fp5-ml
test_flags_axp += 

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

extra_run_test3 = 1000 1000

exes = test1.$(exe) test2.$(exe) test3.$(exe)    

test1.$(exe) : test1.obj field.obj
test2.$(exe) : test2.obj field.obj
test3.$(exe) : test3.obj field.obj

test : .symbolic clean start_test test.lnk $(exes)
    diff exec.out test.chk
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
