plustest_name = exercise

!ifdef test1
test_flags = -d2-xds
alt_error=01
!else ifdef test2
test_flags = -d2-xss
alt_error=02
!else ifdef test3
test_flags = -d2-xdt
alt_error=03
!else ifdef test4
test_flags = -d2-xst
alt_error=04
!else ifdef test5
test_flags = -ox-d1-xds
alt_error=05
!else ifdef test6
test_flags = -ox-d1-xss
alt_error=06
!else ifdef test7
test_flags = -ox-d1-xdt
alt_error=07
!else ifdef test8
test_flags = -d2-xs
alt_error=08
!else ifdef test9
test_flags = -ox-d1-xs
alt_error=09
!else
test_flags = -ox-d1-xst
alt_error=00
!endif

test_flags += -w0

!include testenv.mif

!ifeq alt_error 08
add_br_ = $(br_opt)
!else ifeq alt_error 09
add_br_ = $(br_opt)
!endif

!ifdef %plustest_bad_br
no_br_ctordtor = no
no_br_switcher = no
!endif

test_flags += $(add_br_$(no_br_$^*))

exes = &
    chkcl.$(exe) &
    ctordtor.$(exe) &
    mp.$(exe) &
    ovldall.$(exe) &
    switcher.$(exe) &

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

test : .symbolic start test.lnk $(exes)
    $(noecho)%make common_set_result

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
