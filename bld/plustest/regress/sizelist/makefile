plustest_name = sizelist
!include ../environ.mif

!ifdef test1
test_cflags = -d2
alt_error=01
!else
test_cflags = -ox-d1
alt_error=00
!endif

test_cflags += -zq-w0

test_cflags_386 += $(test_cflags) -zp4-ms
test_cflags_i86 += $(test_cflags) -zp2-ml
test_cflags_axp += $(test_cflags) -zp8

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

.c.obj:
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) -fo=.obj

objs = &
    psfistt.obj

test : .symbolic start_test psfistt.$(exe)
    diff psfistt.out psfistt.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

psfistt.$(exe) : $(objs) psfistt.lnk
    $(linker) @$]@ NAME $@
    $(run) $(exec_prefix)$@ >>psfistt.out

start_test : .symbolic
    %create psfistt.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

psfistt.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))
    @for %i in ( $(objs) ) do @%append $^@ FILE %i

save : .symbolic
    @if exist psfistt.out cp psfistt.out s$(arch)_$(alt_error).sav

global : .SYMBOLIC
    @%make common_clean
