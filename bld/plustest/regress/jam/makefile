plustest_name = jam
!include ../environ.mif

!ifdef test1
test_cflags = -d2
alt_error=01
!else
test_cflags = -otexan-d1
alt_error=00
!endif

test_cflags += -zq-w0-I..

test_cflags_386 += $(test_cflags) -mf
test_cflags_i86 += $(test_cflags) -mm
test_cflags_axp += $(test_cflags)

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

.cpp.obj:
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) -fo=.obj

objs = &
    assert.obj &
    testftor.obj

test : .symbolic start_test jam.$(exe)
    diff jam.out jam.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

jam.$(exe) : $(objs) jam.lnk
    $(linker) @$]@ NAME $@
    $(run) $(exec_prefix)$@ >>jam.out

start_test : .symbolic
    %create jam.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

jam.lnk : makefile
    %create $^@
    @%append $^@ DEBUG line
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))
    @for %i in ( $(objs) ) do @%append $^@ FILE %i

save : .symbolic
    @if exist jam.out cp jam.out s$(arch)_$(alt_error).sav

global : .symbolic
    @%make common_clean
