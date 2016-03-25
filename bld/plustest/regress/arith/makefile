plustest_name = arith

!include testenv1.mif

!ifdef test1
test_flags_386 = -d2-fpd
test_flags_i86 = -d2-fpd
test_flags_axp = -d2
alt_error=01
!else ifdef test2
test_flags_386 = -os-d1-3s-xds $(br_opt)
test_flags_i86 = -os-d1-xds
test_flags_axp = -os-d1-xds
alt_error=02
!else ifdef test3
test_flags_386 = -os-d1-3s-xss-fpd
test_flags_i86 = -os-d1-xss-fpd
test_flags_axp = -os-d1-xss
alt_error=03
!else ifdef test4
test_flags_386 = -ot-d1-3r-fpd -oe1000
test_flags_i86 = -ot-d1-fpd -oe1000
test_flags_axp = -ot-d1 -oe1000
alt_error=04
!else ifdef test5
test_flags_386 = -d2 $(br_opt)
test_flags_i86 = -d2
test_flags_axp = -d2
alt_error=05
!else
test_flags_386 = -os-d1-xst
test_flags_i86 = -os-d1-xst
test_flags_axp = -os-d1-xst
alt_error=00
!endif

test_flags_386 += -fpi87-fp5-mf
test_flags_i86 += -fpi87-fp5-ml
test_flags_axp += 

test_flags = -e1000 -fi=arith.h

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

exes = &
    bench.$(exe)    &
    bitcount.$(exe) &
    calc_e.$(exe)   &
    calc_pi.$(exe)  &
    calc_pi2.$(exe) &
    calc_pi3.$(exe) &
    dmc.$(exe)      &
    mldiv.$(exe)    &
    rsa.$(exe)      &
    waste.$(exe)

test : .symbolic start clean start_test test.lnk $(exes)
    diff $(%ERROR_FILE) test.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

dmc.$(exe) : dmc.obj
    @%make set_path_wlink
    $(linker) @test.lnk NAME $@ FILE $[@
    @%make set_old_path
    @%append $(%ERROR_FILE) --------$^*--------
    tr -d "\r" <dmc.cpp >dmctest.out
    @%make set_path_rtdll
    $(run) $(exec_prefix)$@ c dmctest.out dmc.out >>$(%ERROR_FILE)
    $(run) $(exec_prefix)$@ d dmc.out cpp.out >>$(%ERROR_FILE)
    @%make set_old_path
    diff -b cpp.out dmctest.out >>$(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
