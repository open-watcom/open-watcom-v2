plustest_name = cool

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

test_flags += -I.. -I"../source" -w0 -fi=../__ialias.h

test_flags_i86 = -ml -zm

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

stack_386=opt stack=64k
stack_i86=opt stack=48k
stack_axp=opt stack=64k

opt_link = LIBRARY ../cool.lib

.cpp : .;../source

test : .symbolic start ../cool.lib test.lnk $(exes)
    $(noecho)%make common_set_result

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
    @if exist ../cool.lib $(noecho)rm ../cool.lib
