plustest_name = stl95
!include ../../environ.mif

!ifdef test1
alt_error = 01
test_cflags = -oxa-d1
!else ifdef test2
alt_error = 02
test_cflags = -oxa-d1-xs
!else ifdef test3
alt_error = 03
test_cflags = -d2-xs
!else ifdef test4
alt_error = 04
test_cflags = -oxa-d1-xs
test_cflags_386 = $(br_opt)
test_cflags_i86 =
test_cflags_axp = $(br_opt)
!else ifdef test5
alt_error = 05
test_cflags = -d2-xs
test_cflags_386 = $(br_opt)
test_cflags_i86 =
test_cflags_axp = $(br_opt)
!else
alt_error = 00
test_cflags = -d2
!endif

test_cflags += -w0-zq -I.. -fi=../pch.h -fh

test_cflags_i86 += -mm -zm

extra_istmit1 = <istmit1.in
extra_tech01 = | sed -f fixpath.sed

stack_386=opt stack=64k
stack_i86=opt stack=16k
stack_axp=opt stack=64k

.cpp: ../../stl.94/examples;..
.cpp.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) $(test_cflags_$(arch)) -fo=.obj

.obj.$(exe):
    $(linker) @test.lnk NAME $@ FILE $[@, tempbuf.obj, random.obj
    rm $[@
    %append exec.out @@@ $^@ @@@
    $(run) $(exec_prefix)$@ $(extra_$^*) >>exec.out

!include ../../stl.94/examples/stl_test.mif

test : .symbolic start_test test.lnk tempbuf.obj random.obj $(tests)
!ifeq arch i86
    diff exec.out execi86.chk
!else
    diff exec.out exec.chk
!endif
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create exec.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

test.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .symbolic
    @%make common_clean
