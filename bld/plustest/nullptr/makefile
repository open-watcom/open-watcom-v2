plustest_name = nullptr

!ifdef test1
test_flags = -ms
alt_error=01
!else ifdef test2
test_flags = -mm
alt_error=02
!else ifdef test3
test_flags = -mc
alt_error=03
!else ifdef test4
test_flags = -ml
alt_error=04
!else ifdef test5
test_flags = -mh
alt_error=05
!else
test_flags =
alt_error=00
!endif

!include testenv.mif

exes = nullptr.$(exe)

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

test : .symbolic start test.lnk $(exes)
    $(noecho)%make common_set_result

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
