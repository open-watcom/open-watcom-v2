plustest_name = ehmix

!include testenv1.mif

!ifdef __OS2__
check_file = os2386.chk
!else ifdef __LINUX__
check_file = linux386.chk
!else
check_file = nt386.chk
!endif

!ifdef test1
test_flags = -d2-xs-hd $(br_opt)
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
test_flags = -ox-d1-xs $(br_opt)
alt_error=00
!endif

test_flags += -xr

stack_386=opt stack=64k
stack_i86=opt stack=16k
stack_axp=opt stack=64k

# not posible to compile it for 16-bit due to 
# ehmix.c requires more than 64kB memory for code
#
!ifeq arch i86
test : .symbolic start clean start_test
!else
test : .symbolic start clean start_test test.lnk ehmix.$(exe)
    diff $(%ERROR_FILE) $(check_file)
!endif
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
