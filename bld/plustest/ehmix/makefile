plustest_name = ehmix

!ifdef test1
test_flags = -d2-xs-hd
alt_error=01
!else ifdef test2
test_flags = -d2-xss-hd
alt_error=02
!else ifdef test3
test_flags = -d2-xst-hd
alt_error=03
!else ifdef test4
test_flags = -ox-d1-xst
alt_error=04
!else ifdef test5
test_flags = -ox-d1-xss
alt_error=05
!else
test_flags = -ox-d1-xs
alt_error=00
!endif

test_flags += -xr

!ifdef __OS2__
exec_chk_386 = os2386.chk
!else ifdef __LINUX__
exec_chk_386 = linux386.chk
!else
exec_chk_386 = nt386.chk
!endif

!include testenv.mif

!ifeq alt_error 00
test_flags += $(br_opt)
!else ifeq alt_error 01
test_flags += $(br_opt)
!endif

stack_386=opt stack=64k
stack_i86=opt stack=16k
stack_axp=opt stack=64k

# not posible to compile it for 16-bit due to
# ehmix.c requires more than 64kB memory for code
#
!ifeq arch i86
test : .symbolic start
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean
!else
test : .symbolic start test.lnk ehmix.$(exe)
    $(noecho)%make common_set_result_display
!endif

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
