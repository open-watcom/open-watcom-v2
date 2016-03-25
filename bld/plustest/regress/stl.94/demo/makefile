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

test_flags += -w0 -I..

test_flags_i86 = -mm -zm

stack_386=opt stack=128k
stack_i86=opt stack=32k
stack_axp=opt stack=128k

extra_run_demo = <demo.in

.cpp: .;..

#
# function code overflow 16-bit segment
# don't use it for i86 target
#

!ifeq arch i86
test : .symbolic start clean start_test
!else
test : .symbolic start clean start_test test.lnk demo.$(exe)
    diff $(%ERROR_FILE) demo.chk
!endif
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

demo.$(exe) : demo.obj tempbuf.obj

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm -f s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
