plustest_name = stl94

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

!include testenv.mif

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
test : .symbolic start
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean
!else
test : .symbolic start test.lnk demo.$(exe)
    $(noecho)%make common_set_result_display
!endif

demo.$(exe) : demo.obj tempbuf.obj

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
