plustest_name = splash
!include ../environ.mif

objs = &
assoc.obj &
chgfnt.obj &
cislog.obj &
justify.obj &
regex.obj &
splash.obj &
spltest.obj &
tracer.obj &
tstio.obj &
xcl.obj

!ifdef test1
test_cflags = -d2
alt_error=01
!else ifdef test2
test_cflags = -ox-d1
alt_error=02
!else
test_cflags = -ox-d1 -oe1000
alt_error=00
!endif
test_cflags += -zq-w0-dTEST

test_cflags_386 = $(test_cflags) -3s
test_cflags_i86 = $(test_cflags) -mm
test_cflags_axp = $(test_cflags)

stack_386=opt stack=64k
stack_i86=opt stack=8k
stack_axp=opt stack=64k

.c.obj:
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) -fo=.obj

exes = &
    splassoc.$(exe) &
    justify.$(exe) &
    spltest.$(exe)

test : .symbolic start_test $(exes)
    diff exec.out exec.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

spltest.$(exe) : spltest.obj splash.obj regex.obj test.lnk
    $(linker) @test.lnk NAME $^@ FILE spltest.obj, splash.obj, regex.obj
    @%append exec.out --------$^&--------
    $(run) $(exec_prefix)$@ >>exec.out

splassoc.$(exe) : assoc.obj splash.obj regex.obj test.lnk
    $(linker) @test.lnk NAME $^@ FILE assoc.obj, splash.obj, regex.obj
    @%append exec.out --------$^&--------
    $(run) $(exec_prefix)$@ >>exec.out

justify.$(exe) : justify.obj splash.obj regex.obj test.lnk
    $(linker) @test.lnk NAME $^@ FILE justify.obj, splash.obj, regex.obj
    @%append exec.out --------$^&--------
    $(run) $(exec_prefix)$@ 13 <justify.c >>exec.out

start_test : .symbolic
    %create exec.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

regex.obj : regex.c
    $(wcc_$(arch)) $[* $(test_cflags_$(arch)) -fo=.obj

test.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .symbolic
    @%make common_clean
