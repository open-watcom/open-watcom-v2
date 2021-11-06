plustest_name = format

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags_axp =
test_flags_386 = -mf
test_flags_i86 = -mc

opt_link_testi = FILE iformat.obj
opt_link_testf = FILE fformat.obj

!include testenv.mif

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

exes = testi.$(exe) testf.$(exe)

test : .symbolic start test.lnk iformat.obj fformat.obj $(exes)
    $(noecho)%make common_set_result

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
