plustest_name = arith
!include ../environ.mif

!ifdef test1
test_cflags_386 = -d2-fpd
test_cflags_i86 = -d2-fpd
test_cflags_axp = -d2
alt_error=01
!else ifdef test2
test_cflags_386 = -os-d1-3s-xds $(br_opt)
test_cflags_i86 = -os-d1-xds
test_cflags_axp = -os-d1-xds
alt_error=02
!else ifdef test3
test_cflags_386 = -os-d1-3s-xss-fpd
test_cflags_i86 = -os-d1-xss-fpd
test_cflags_axp = -os-d1-xss
alt_error=03
!else ifdef test4
test_cflags_386 = -ot-d1-3r-fpd -oe1000
test_cflags_i86 = -ot-d1-fpd -oe1000
test_cflags_axp = -ot-d1 -oe1000
alt_error=04
!else ifdef test5
test_cflags_386 = -d2 $(br_opt)
test_cflags_i86 = -d2
test_cflags_axp = -d2
alt_error=05
!else
test_cflags_386 = -os-d1-xst
test_cflags_i86 = -os-d1-xst
test_cflags_axp = -os-d1-xst
alt_error=00
!endif

test_cflags_386 += -zq-fpi87-fp5-mf
test_cflags_i86 += -zq-fpi87-fp5-ml
test_cflags_axp += -zq

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

.cpp.obj:
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) -e1000 -fi=arith.h -fo=.obj

.obj.$(exe):
    $(linker) @test.lnk NAME $@ FILE $[@
    @%append test.out --------$^*--------
    $(run) $(exec_prefix)$@ >>test.out

dmc.$(exe) : dmc.obj
    $(linker) @test.lnk NAME $@ FILE $[@
    @%append test.out --------$^*--------
!ifdef __UNIX__
    # it converts \n to \r\n by the help of temporary \xFF character
    cat dmc.cpp | tr '\n' '\377' | sed -e 's/\xFF/\r\n/g' >dmctest.out
!else
    cp dmc.cpp dmctest.out
!endif
    $(run) $(exec_prefix)$@ c dmctest.out dmc.out >>test.out
    $(run) $(exec_prefix)$@ d dmc.out cpp.out >>test.out
    diff cpp.out dmctest.out >>test.out

test : .symbolic start_test test.lnk $(exes)
    diff test.out test.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create test.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

test.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist test.out cp test.out s$(arch)_$(alt_error).sav

global : .procedure
    @%make common_clean
