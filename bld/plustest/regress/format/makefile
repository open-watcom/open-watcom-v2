plustest_name = format
!include ../environ.mif

!ifdef test1
test_cflags = -d2
alt_error=01
!else
test_cflags = -ox-d1
alt_error=00
!endif

test_cflags += -zq

test_cflags_axp = $(test_cflags)
test_cflags_386 = $(test_cflags) -mf
test_cflags_i86 = $(test_cflags) -mc

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

.c.obj:
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) -fo=.obj

i_objs = iformat.obj itest.obj
f_objs = fformat.obj ftest.obj

exes = testi.$(exe) testf.$(exe)

test : .symbolic start_test $(exes)
    diff exec.out exec.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

testi.$(exe) : $(i_objs) test.lnk
    $(linker) @$]@ NAME $@ file {$(i_objs)}
    @%append exec.out --------$^&--------
    $(run) $(exec_prefix)$@ >>exec.out

testf.$(exe) : $(f_objs) test.lnk
    $(linker) @$]@ NAME $@ file {$(f_objs)}
    @%append exec.out --------$^&--------
    $(run) $(exec_prefix)$@ >>exec.out

start_test : .symbolic
    %create exec.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

test.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION verbose

save : .symbolic
    @if exist testf.out cp testf.out s$(arch)_$(alt_error).sav

global : .SYMBOLIC
    @%make common_clean
