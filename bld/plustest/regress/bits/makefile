plustest_name = bits
!include ../environ.mif

!ifdef test1
test_cflags_386 = -d2-3r
test_cflags_i86 = -d2
test_cflags_axp = -d2
alt_error=01
!else
test_cflags_386 = -ox-d1-3s
test_cflags_i86 = -ox-d1
test_cflags_axp = -ox-d1
alt_error=00
!endif

test_cflags_386 += -zq-mf
test_cflags_i86 += -zq-ml
test_cflags_axp += -zq

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

.cpp.obj:
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) -fo=.obj

objs = tbits.obj
    
test : .symbolic start_test tbits.$(exe)
    diff tbits.out tbits.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

tbits.$(exe) : $(objs) tbits.lnk
    $(linker) @$]@ name $@ file {$(objs)}
    $(run) $(exec_prefix)$@ >>tbits.out

start_test : .symbolic
    %create tbits.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

tbits.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))
    
save : .symbolic
    @if exist tbits.out cp tbits.out s$(arch)_$(alt_error).sav

global : .procedure
    @%make common_clean
