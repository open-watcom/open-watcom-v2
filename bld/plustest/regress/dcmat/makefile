plustest_name = dcmat
!include ../environ.mif

objs = &
    compmat.obj &
    cosft.obj &
    doubmat.obj &
    four1.obj &
    realft.obj &
    svdcmp.obj

!ifdef test1
test_cflags = -d2
alt_error=01
!else
test_cflags = -ox-d1
alt_error=00
!endif

test_cflags += -zq -e10 -w0 -D__ATT_complex__ -I.

test_cflags_386 =
test_cflags_i86 = -mm -zm
test_cflags_axp =

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

.obj.$(exe):
    $(linker) @test.lnk name $@ FILE $[@
    $(run) $(exec_prefix)$@ >>test.out

.c.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) $(test_cflags_$(arch)) -fo=.obj

exes = test.$(exe) tmatrix.$(exe)

test : .symbolic start_test matrix.lib test.lnk $(exes)
    diff test.out test.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create test.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

matrix.lib : $(objs) matrix.lbc
    $(librarian_$(arch)) -q-n-b $@ @matrix.lbc

matrix.lbc : makefile
    %create $@
    @for %i in ( $(objs) ) do @%append $@ -+%i

test.lnk : makefile
    %create $@
    @%append $@ $(ldebug_$(arch))
    @%append $@ $(lnk_$(arch))
    @%append $@ $(stack_$(arch))
    @%append $@ LIBRARY matrix.lib

save : .symbolic
    @if exist test.out cp test.out s$(arch)_$(alt_error).sav

global : .procedure
    @%make common_clean
