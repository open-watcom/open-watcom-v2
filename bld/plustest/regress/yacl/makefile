#pmake: clean

plustest_name = yacl

!include testenv1.mif
!include base/lib.mif
!include io/lib.mif

exes = &
bittest.$(exe) &
btreetst.$(exe) &
bytstrm.$(exe) &
dkbtrtst.$(exe) &
format1.$(exe) &
gseqtest.$(exe) &
holidays.$(exe) &
maptest.$(exe) &
perstest.$(exe) &
seqtest.$(exe) &
settest.$(exe) &
slotftst.$(exe) &
strset.$(exe) &
strtest.$(exe) &
treeprnt.$(exe) &
treetest.$(exe) &
wordcnt.$(exe)

!ifdef test1
test_flags = -ot-op-d1
alt_error=01
!else ifdef test2
test_flags = -ot-op-d1-xds
alt_error=02
!else ifdef test3
test_flags = -op-d2-xds
alt_error=03
!else
test_flags = -op-d2
alt_error=00
!endif

test_flags += -ei -I.. -w0 -D__OS2__

test_flags_i86 = -ml

stack_386=opt stack=32k
stack_i86=opt stack=24k
stack_axp=opt stack=32k

opt_link = LIBRARY base.lib, io.lib

extra_run_format1 = <test/format1.cpp
extra_run_holidays = 1999
extra_run_wordcnt = <test/wordcnt.cpp

.c : test;base;io
.cpp : test;base;io

test : .symbolic start clean start_test base.lib io.lib test.lnk $(exes)
    diff $(%ERROR_FILE) test.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

dkbtrtst.$(exe) : dkbtrtst.obj
    @set ERROR_MSG=*** failure to link $^&
    @%make set_path_wlink
    $(linker) @test.lnk NAME $@ FILE $[@ $(opt_link)
    @%append $(%ERROR_FILE) PASS compiling $^&
    @set ERROR_MSG=*** failure to execute $^&
    @%make set_path_rtdll
    $(run) $(exec_prefix)$@ >>$(%ERROR_FILE)
    @%append $(%ERROR_FILE) PASS executing $^&
    $(run) $(exec_prefix)$@ >>$(%ERROR_FILE)
    @%make set_old_path
    @%append $(%ERROR_FILE) PASS executing $^&

perstest.$(exe) : perstest.obj
    @set ERROR_MSG=*** failure to link $^&
    @%make set_path_wlink
    $(linker) @test.lnk NAME $@ FILE $[@ $(opt_link)
    @%append $(%ERROR_FILE) PASS compiling $^&
    @set ERROR_MSG=*** failure to execute $^&
    @%make set_path_rtdll
    $(run) $(exec_prefix)$@ >>$(%ERROR_FILE)
    @%append $(%ERROR_FILE) PASS executing $^&
    $(run) $(exec_prefix)$@ >>$(%ERROR_FILE)
    @%make set_old_path
    @%append $(%ERROR_FILE) PASS executing $^&

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @if exist *.dat rm -f *.dat
    @if exist base.lib rm -f base.lib
    @if exist io.lib rm -f io.lib
    @%make common_clean
