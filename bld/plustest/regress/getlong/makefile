plustest_name = getlong
!include ../environ.mif

!ifdef test1
test_cflags = -d2
alt_error=01
!else
test_cflags = -ox-d1
alt_error=00
!endif

test_cflags += -zq

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

.cc.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) -fo=.obj

objs = &
    getlongo.obj &
    readme.obj

test : .symbolic start_test opt.$(exe)
    diff opt.out opt.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

opt.$(exe) : $(objs) opt.lnk
    $(linker) @$]@ NAME $@
    $(run) $(exec_prefix)$@ >>opt.out
    $(run) $(exec_prefix)$@ -h >>opt.out
    -$(run) $(exec_prefix)$@ -hopeless >>opt.out
    $(run) $(exec_prefix)$@ -vers >>opt.out
    $(run) $(exec_prefix)$@ -v >>opt.out
    -$(run) $(exec_prefix)$@ -o >>opt.out
    -$(run) $(exec_prefix)$@ -v=1 >>opt.out

start_test : .symbolic
    %create opt.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

opt.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))
    @for %i in ( $(objs) ) do @%append $^@ FILE %i

save : .symbolic
    @if exist opt.out cp opt.out s$(arch)_$(alt_error).sav

global : .SYMBOLIC
    @%make common_clean
