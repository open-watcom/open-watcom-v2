plustest_name = field64
!include ../environ.mif

!ifdef test1
test_cflags_386 = -d2-fpd
test_cflags_i86 = -d2-fpd-zm
test_cflags_axp = -d2
alt_error=01
!else ifdef test2
test_cflags_386 = -os-d1-3s-xds
test_cflags_i86 = -os-d1-xds
test_cflags_axp = -os-d1-xds
alt_error=02
!else ifdef test3
test_cflags_386 = -os-d1-3s-xss-fpd
test_cflags_i86 = -os-d1-xss-fpd
test_cflags_axp = -os-d1-xss
alt_error=03
!else
test_cflags_386 = -os-d1-xst
test_cflags_i86 = -os-d1-xst
test_cflags_axp = -os-d1-xst
alt_error=00
!endif

test_cflags_386 += -zq-fpi87-fp5-mf
test_cflags_i86 += -zq-fpi87-fp5-ml-s
test_cflags_axp += -zq

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

.cpp.obj:
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) -e1000 -fo=.obj

exes = test1.$(exe) test2.$(exe) test3.$(exe)

test : .symbolic start_test $(exes)
    diff test.out test.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

test1.$(exe) : field.obj test1.obj field.lnk
    $(linker) @$]@ NAME $@ FILE test1.obj, field.obj
    $(run) $(exec_prefix)$@ >>test.out

test2.$(exe) : field.obj test2.obj field.lnk
    $(linker) @$]@ NAME $@ FILE test2.obj, field.obj
    $(run) $(exec_prefix)$@ >>test.out

test3.$(exe) : field.obj test3.obj field.lnk
    $(linker) @$]@ NAME $@ FILE test3.obj, field.obj
    $(run) $(exec_prefix)$@ 1000 1000 >>test.out

start_test : .symbolic
    %create test.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

field.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist test.out cp test.out s$(arch)_$(alt_error).sav

global : .SYMBOLIC
    @%make common_clean
