plustest_name = jam

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -otexan-d1
alt_error=00
!endif

test_flags += -w0 -I..

test_flags_386 = -mf
test_flags_i86 = -mm
test_flags_axp =

opt_link_testftor = FILE assert.obj

!include testenv.mif

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

exes = testftor.$(exe)

test : .symbolic start test.lnk assert.obj $(exes)
    $(noecho)%make common_set_result

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
