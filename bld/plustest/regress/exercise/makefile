plustest_name = exercise

!include testenv.mif

exes = &
    chkcl.$(exe) &
    ctordtor.$(exe) &
    mp.$(exe) &
    ovldall.$(exe) &
    switcher.$(exe) &

!ifdef test1
test_flags += -d2-xds
alt_error=01
!else ifdef test2
test_flags += -d2-xss
alt_error=02
!else ifdef test3
test_flags += -d2-xdt
alt_error=03
!else ifdef test4
test_flags += -d2-xst
alt_error=04
!else ifdef test5
test_flags += -ox-d1-xds
alt_error=05
!else ifdef test6
test_flags += -ox-d1-xss
alt_error=06
!else ifdef test7
test_flags += -ox-d1-xdt
alt_error=07
!else ifdef test8
test_flags += -d2-xs
add_br_ = $(br_opt)
alt_error=08
!else ifdef test9
test_flags += -ox-d1-xs
add_br_ = $(br_opt)
alt_error=09
!else
test_flags += -ox-d1-xst
alt_error=00
!endif

test_flags += -w0

!ifdef %plustest_bad_br
no_br_ctordtor = no
no_br_switcher = no
!endif

!ifeq arch 386
test_flags += $(add_br_$(no_br_$^*))
!endif

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

test : .symbolic start clean start_test test.lnk $(exes)
    diff $(%ERROR_FILE) exercise.chk
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
