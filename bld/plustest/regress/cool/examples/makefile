plustest_name = cool

!include testenv.mif
!include ../lib.mif

exes = &
    ex2_4.$(exe) &
    ex2_7.$(exe) &
    ex2_9.$(exe) &
    ex3_4.$(exe) &
    ex3_6.$(exe) &
    ex3_8.$(exe) &
    ex3_10.$(exe) &
#    ex4_7.$(exe) &   # it is incompatible with OW timezone stuff, fix it!
    ex4_9.$(exe) &
    ex5_8.$(exe) &
    ex6_4.$(exe) &
    ex6_6.$(exe) &
    ex6_8.$(exe) &
    ex6_10.$(exe) &
    ex7_4.$(exe) &
    ex7_7.$(exe) &
    ex7_9.$(exe) &
    ex8_4.$(exe) &
    ex8_6.$(exe) &
    ex9_5.$(exe) &
    ex9_7.$(exe) &
    ex9_11.$(exe)

!ifdef test1
test_flags = -op-d2
alt_error=01
!else ifdef test2
test_flags = -ot-op-d1-xds
alt_error=02
!else ifdef test3
test_flags = -op-d2-xds
alt_error=03
!else
test_flags = -ot-op-d1
alt_error=00
!endif

test_flags += -I"../.." -w0
!ifdef __UNIX__
test_flags += -fi=../__ialias.h
!endif

test_flags_i86 = -ml -zm

stack_386=opt stack=64k
stack_i86=opt stack=48k
stack_axp=opt stack=64k

opt_link = LIBRARY ../cool.lib

.cpp : .;..

test : .symbolic start clean start_test ../cool.lib test.lnk $(exes)
    diff $(%ERROR_FILE) exec.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @if exist ../cool.lib rm ../cool.lib
    @%make common_clean
