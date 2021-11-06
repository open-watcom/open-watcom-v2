plustest_name = sizelist

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags += -w0

test_flags_386 = -zp4-ms
test_flags_i86 = -zp2-ml
test_flags_axp = -zp8

!include testenv.mif

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

exes = psfistt.$(exe)

test : .symbolic start test.lnk $(exes)
    $(noecho)%make common_set_result

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
