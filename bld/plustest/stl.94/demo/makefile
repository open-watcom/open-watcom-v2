plustest_name = stl94

!include testenv1.mif

!ifdef test1
test_flags = -oxa-d1-hd
alt_error=01
!else ifdef test2
test_flags = -oxa-d1-xs-hd
alt_error=02
!else ifdef test3
test_flags = -d2-xs-hd
alt_error=03
!else
test_flags = -d2-hd
alt_error=00
!endif

test_flags += -w0 -I../source

test_flags_i86 = -mm -zm

stack_386=opt stack=128k
stack_i86=opt stack=32k
stack_axp=opt stack=128k

extra_run_demo = <demo.in

.cpp: .;../source

#
# function code overflow 16-bit segment
# don't use it for i86 target
#

!ifeq arch i86
test : .symbolic start clean start_test set_result
!else
test : .symbolic start clean start_test test.lnk demo.$(exe) set_result
!endif
    @%make clean

set_result : .symbolic
!ifeq arch i86
!else
    $(noecho)%make set_fail
    $(noecho)%make common_save_sav
    diff $(sav_file) exec.chk
    $(noecho)%make common_clean_sav
!endif
    $(noecho)%make set_pass

demo.$(exe) : demo.obj tempbuf.obj

start_test : .symbolic
    @%make common_start_test

clean : .procedure
    $(noecho)%make common_clean
