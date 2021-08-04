plustest_name = field

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

opt_link_test1 = FILE field.obj
opt_link_test2 = FILE field.obj
opt_link_test3 = FILE field.obj

!include testenv.mif

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

extra_run_test3 = 1000 1000

exes = test1.$(exe) test2.$(exe) test3.$(exe)

test : .symbolic start test.lnk field.obj $(exes)
    $(noecho)%make common_set_result

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
