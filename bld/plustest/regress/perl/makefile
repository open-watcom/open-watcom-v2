plustest_name = perl
!include ../environ.mif

objs = &
chgfnt.obj &
cislog.obj &
perlasso.obj &
perlclas.obj &
perltest.obj &
regex.obj &
tstio.obj

exes = &
    perltest.$(exe) &
    perlasso.$(exe) &
    tstio.$(exe)

!ifdef test1
test_cflags = -d2
alt_error=01
!else
test_cflags = -ox-d1
alt_error=00
!endif

test_cflags += -zq-w0

test_cflags_386 =
test_cflags_i86 = -mm
test_cflags_axp =

stack_386=opt stack=32k
stack_i86=opt stack=16k
stack_axp=opt stack=32k

.cpp.obj:
    $(wpp_$(arch)) $[* $(test_cflags) $(test_cflags_$(arch)) -dTEST -fo=.obj

.c.obj:
    $(wcc_$(arch)) $[* $(test_cflags) $(test_cflags_$(arch)) -fo=.obj

test : .symbolic start_test $(exes)
    diff exec.out exec.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

perltest.$(exe) : perltest.obj perlclas.obj regex.obj test.lnk
    $(linker) @test.lnk NAME $@ FILE perltest.obj, perlclas.obj, regex.obj
    @%append exec.out --------$^&--------
    $(run) $(exec_prefix)$@ >>exec.out

perlasso.$(exe) : perlasso.obj perlclas.obj regex.obj test.lnk
    $(linker) @test.lnk NAME $@ FILE perlasso.obj, perlclas.obj, regex.obj
    @%append exec.out --------$^&--------
    $(run) $(exec_prefix)$@ >>exec.out

tstio.$(exe) : tstio.obj perlclas.obj regex.obj test.lnk
    $(linker) @test.lnk NAME $@ FILE tstio.obj, perlclas.obj, regex.obj
    @%append exec.out --------$^&--------
    $(run) $(exec_prefix)$@ <tstio.in >>exec.out
    @%append exec.out --------t2--------
    cat t2.out >>exec.out

start_test : .symbolic
    %create exec.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

test.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .symbolic
    @%make common_clean
