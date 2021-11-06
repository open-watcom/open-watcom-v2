plustest_name = arith

test_flags = -e1000 -fi=arith.h

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
!else ifdef test4
test_flags_386 = -ot-d1-3r-fpd -oe1000
test_flags_i86 = -ot-d1-fpd -oe1000
test_flags_axp = -ot-d1 -oe1000
alt_error=04
!else ifdef test5
test_flags_386 = -d2
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

!include testenv.mif

!ifeq alt_error 02
test_flags += $(br_opt)
!else ifeq alt_error 05
test_flags += $(br_opt)
!endif

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

test : .symbolic start test.lnk $(exes)
    $(noecho)%make common_set_result

dmc.$(exe) : dmc.obj
    @%make set_path_wlink
    @%make echo_link
    $(noecho)$(linker) @test.lnk NAME $@ FILE $[@
    @%make set_old_path
    @%append $(%ERROR_FILE) --------$^*--------
    -$(noecho)crlf -u dmc.cpp dmctest.out
    @%make set_run_path
    -$(run) $(exec_prefix)$@ c dmctest.out dmc.out >>$(%ERROR_FILE)
    -$(run) $(exec_prefix)$@ d dmc.out cpp.out >>$(%ERROR_FILE)
    @%make set_old_run_path
    -diff -b cpp.out dmctest.out >>$(%ERROR_FILE)

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
