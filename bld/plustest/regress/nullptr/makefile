plustest_name = nullptr
!include ../environ.mif

exes = nullptr.$(exe)

!ifdef test1
test_cflags += -ms
alt_error=01
!else ifdef test2
test_cflags += -mm
alt_error=02
!else ifdef test3
test_cflags += -mc
alt_error=03
!else ifdef test4
test_cflags += -ml
alt_error=04
!else ifdef test5
test_cflags += -mh
alt_error=05
!else
alt_error=00
!endif


stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

.cpp.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) -fo=.obj

.obj.$(exe):
    $(linker) @test.lnk name $@ FILE $[@
    $(run) $(exec_prefix)$@ >>nullptr.out

test : .symbolic start_test test.lnk $(exes)
    diff nullptr.out nullptr.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create nullptr.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

test.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ DEBUG line
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist nullptr.out cp exercise.out s$(arch)_$(alt_error).sav

global : .SYMBOLIC
    @%make common_clean
