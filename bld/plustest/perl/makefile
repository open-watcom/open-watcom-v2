plustest_name = perl

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags += -w0 -DTEST

test_flags_386 =
test_flags_i86 = -mm
test_flags_axp =

opt_link_perltest = FILE { perlclas.obj regex.obj }
opt_link_perlasso = FILE { perlclas.obj regex.obj }
opt_link_tstio = FILE { perlclas.obj regex.obj }

!include testenv.mif

stack_386=opt stack=32k
stack_i86=opt stack=16k
stack_axp=opt stack=32k

extra_run_tstio = <tstio.in

exes = &
    perltest.$(exe) &
    perlasso.$(exe) &
    tstio.$(exe)

test : .symbolic start test.lnk perlclas.obj regex.obj $(exes) test_set_result
    $(noecho)%make common_set_result

test_set_result : .symbolic
    cat t2.out >>$(%ERROR_FILE)

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
